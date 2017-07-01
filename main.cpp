#include <iostream>
#include <string.h>
#include <math.h>
#include <random>
#include <time.h>
#include <iomanip>
#include <fstream>
using namespace std;


class eventList{
    private:
        struct eventNode{
            double time;
            char type;
            eventNode* next;
            eventNode* prev;

        };
        eventNode* head;
    public:
        eventList();
        ~eventList();
        void clearList();
        bool put(double t,char x,double timeC);
        void print();
        char get(double& timeC);
};

eventList::eventList(){
    head=NULL;
}

eventList::~eventList(){
    clearList();
}

void eventList::clearList(){
    eventNode* aux=head;

    while (aux!=NULL){
        head=head->next;
        delete aux;
        aux=head;
    }
}

bool eventList::put(double t,char x,double timeC){
    if(t<=timeC)
        return false;
    eventNode* newNode=new eventNode;
    newNode->time=t;
    newNode->type=x;

    if (head==NULL){
        head=newNode;
        head->next=NULL;
        head->prev=NULL;
        return true;
    }

    if (head->time>t){
        newNode->next=head;
        newNode->prev=NULL;
        head=newNode;
        head->next->prev=head;
        return true;
    }

    eventNode* temp=head;
    while(temp->next!=NULL){
        if(temp->next->time>t){
            newNode->next=temp->next;
            temp->next=newNode;
            newNode->prev=temp;
            newNode->next->prev=newNode;
            return true;
        }
        temp=temp->next;
    }
    temp->next=newNode;
    newNode->next=NULL;
    newNode->prev=temp;
    return true;
}

void eventList::print(){
    if(head==NULL){
        cout<<"The queue is empty\n\n";
        return;
    }
    eventNode* temp=head;

    cout<<"The queue has the following elements: \n\n";

    while(temp!=NULL){
        cout<<"Time="<<temp->time<<"\nEvent type="<<temp->type<<endl<<endl;
        temp=temp->next;
    }
}

char eventList::get(double& timeC){
    if(head==NULL)
        return '0';
    eventNode* aux=head;
    char aux2=head->type;
    timeC=head->time;
    head=head->next;
    delete aux;
    return aux2;
}


int main(){
    double customersServed1=0;
    double customersServed2=0;
    double currentTime=0;
    int N1=0; //elements in the first part of the system(queue1 + server1)
    int N2=0; //elements in the second part of the system(queue2 + server2)
    double lengthQueue1, lengthQueue2;
    double blockedElements1=0;
    double blockedElements2=0;
    double s = 0.0; // To calculate mean number of elements in the system
    double a1 = 0.0; // To calculate server 1 utilization rate
    double a2 = 0.0; // To calculate server 2 utilization rate
    double previousTime;
    double previousUsageStart1;
    double previousUsageStart2;
    double lambda,mu,mu2,p1,p2;
    eventList x;
    int customersToSimulate=100000;
    int customersArrived=0;
    bool isFinished=false;
    int replications=10;
    int auxReplications=0;
    ofstream myfile;
    myfile.open ("simulationResults.txt");

    do{
        cout<<"Enter the mean arrival rate (in elements/time unit):\n";
        cin>>lambda;
    }while (lambda<0);

    while(auxReplications<replications){
        mu=2;
        mu2=1;
        lengthQueue1=5;
        lengthQueue2=5;
        p1=lambda/mu;


        std::default_random_engine generator;
        generator.seed(time(NULL)+auxReplications);
        std::exponential_distribution<double> distributionA(lambda);
        std::exponential_distribution<double> distributionM(mu);
        std::exponential_distribution<double> distributionD(mu2);

        x.put(1,'A',currentTime);

        while (!isFinished){
            previousTime=currentTime; //used to calculate the mean number of customers in the system
            switch(x.get(currentTime)){
            case 'A':
                customersArrived++;
                s=s+(N1+N2)*(currentTime-previousTime);
                if (N1>lengthQueue1){
                    blockedElements1++;
                    if (customersArrived<customersToSimulate){
                        x.put(currentTime+distributionA(generator),'A',currentTime);
                    }
                    break;
                }
                else if (N1==0){
                    x.put(currentTime+distributionM(generator),'M',currentTime);
                    previousUsageStart1=currentTime;// To calculate server  utilization rate
                }
                N1++;
                if (customersArrived<customersToSimulate){
                    x.put(currentTime+distributionA(generator),'A',currentTime);
                }
                break;
            case 'M':
                s=s+(N1+N2)*(currentTime-previousTime);
                a1=a1+(currentTime-previousUsageStart1);
                N1--;
                customersServed1++;
                if(N2>lengthQueue2){
                    blockedElements2++;
                    if (N1>0){
                        x.put(currentTime+distributionM(generator),'M',currentTime);
                        previousUsageStart1=currentTime;
                    }
                    break;
                }
                else if(N2==0){
                    x.put(currentTime+distributionD(generator),'D',currentTime);
                    previousUsageStart2=currentTime; // To calculate server  utilization rate
                }
                N2++;
                if (N1>0){
                    x.put(currentTime+distributionM(generator),'M',currentTime);
                    previousUsageStart1=currentTime;
                }
                break;
            case 'D':
                s=s+(N1+N2)*(currentTime-previousTime);
                N2--;
                customersServed2++;
                a2=a2+(currentTime-previousUsageStart2);
                if(N2>0){
                    x.put(currentTime+distributionD(generator),'D',currentTime);
                    previousUsageStart2=currentTime;
                }
                else if (N1==0 && N2==0 && customersArrived==customersToSimulate)
                    isFinished=true;
                break;
            }
        }

        p2=(customersServed1/currentTime)/mu2;
        cout<<"Blocked elements in queue1: "<<blockedElements1<<endl;
        double dropRate1=blockedElements1/(customersArrived)*100;
        cout<<"Blocking probability in queue1: "<<dropRate1<<"%\n";
        double TdropRate1=100*((1.001-p1)*pow(p1,lengthQueue1+1))/(1.001-pow(p1,lengthQueue1+2));
        cout<<"Theoretical blocking probability in queue1: "<<TdropRate1<<"%\n\n";
        cout<<"Blocked elements in queue2: "<<blockedElements2<<endl;
        double dropRate2=blockedElements2/(blockedElements2+customersServed2)*100;
        cout<<"Blocking probability in queue2: "<<dropRate2<<"%\n";
        double TdropRate2=100*((1.001-p2)*pow(p2,lengthQueue2+1))/(1.001-pow(p2,lengthQueue2+2));
        cout<<"Theoretical blocking probability in queue2: "<<TdropRate2<<"%\n\n";
        double totalDropRate=(blockedElements1+blockedElements2)/(customersToSimulate)*100;
        cout<<"Total blocking probability: "<<totalDropRate<<"%\n\n";
        double Trate=customersServed2 / currentTime;
        cout<<"Throughput Rate: "<<Trate<<" elements/s\n\n";
        double meanElements=s / currentTime;
        cout<<"Mean number of elements in the system: "<<meanElements<<endl<<endl;
        double sojournT=meanElements/Trate;
        cout<<"Mean Sojourn time: "<<sojournT<<"s\n\n";
        double uRate1=a1/currentTime*100;
        cout<<"Server 1 utilization rate: "<<uRate1<<"%\n\n";
        double uRate2=a2/currentTime*100;
        cout<<"Server 2 utilization rate: "<<uRate2<<"%\n\n";
        if (auxReplications==0){
            myfile<<"Simulation nº\tBlocked elements in queue 1\tBlocking Probability Queue 1\tTheoretical BP Queue 1\tBlocked elements in queue 2\t";
            myfile<<"Blocking Probability Queue 2\tTheoretical BP Queue 2\tTotal Blocking Probability\tThroughput Rate\t";
            myfile<<"Mean nº of elements in the system\tMean Sojourn Time\tServer 1 utilization rate\tServer 2 utilization rate\n";
        }
        myfile<<std::fixed << std::setprecision(3) <<auxReplications+1<<"\t"<<blockedElements1<<"\t"<<dropRate1<<"%\t"<<TdropRate1<<"%\t"<<blockedElements2<<"\t"<<dropRate2<<"%\t"<<TdropRate2;
        myfile<<"%\t"<<totalDropRate<<"%\t"<<Trate<<" elements/s\t"<<meanElements<<"\t"<<sojournT<<" s\t"<<uRate1<<"%\t"<<uRate2<<"%\n";
        N1=0;
        N2=0;
        s=0;
        customersServed1=0;
        customersServed2=0;
        blockedElements1=0;
        blockedElements2=0;
        a1=0;
        a2=0;
        currentTime=0;
        customersArrived=0;
        x.clearList();
        isFinished=false;
        auxReplications++;

    }
    myfile.close();
    return 0;
}


