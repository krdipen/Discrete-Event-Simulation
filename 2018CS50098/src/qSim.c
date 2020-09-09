#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include<math.h>

typedef struct Event{
    float time;
    int id;
    float arrTime;
    float startExe;
    float stopExe;
}Event;

typedef struct Node{
    Event *event;
    struct Node *next;
}Node;

typedef struct Queue{
    Node *head;
    Node *tail;
}Queue;

void pushBack(Queue *queue,Event *event){
    Node *node=(Node*)malloc(sizeof(Node));
    node->event=event;
    node->next=NULL;
    if(queue->tail==NULL){
        queue->head=node;
        queue->tail=node;
    }
    else{
        queue->tail->next=node;
        queue->tail=node;
    }
}

void pushTime(Queue *queue,Event *event){
    Node *node=(Node*)malloc(sizeof(Node));
    node->event=event;
    node->next=NULL;
    if(queue->head==NULL){
        queue->head=node;
        queue->tail=node;
    }
    else{
        Node *previous=NULL;
        Node *current=queue->head;
        while(current->event->time<=node->event->time){
            previous=current;
            current=current->next;
            if(current==NULL){
                queue->tail->next=node;
                queue->tail=node;
                return;
            }
        }
        if(previous==NULL){
            queue->head=node;
            node->next=current;
        }
        else{
            previous->next=node;
            node->next=current;
        }
    }
}

Event* pop(Queue *queue){
    if(queue->head==NULL){
        return NULL;
    }
    else{
        Event *event=queue->head->event;
        if(queue->head->next==NULL){
            queue->head=NULL;
            queue->tail=NULL;
        }
        else{
            queue->head=queue->head->next;
        }
        return event;
    }
}

void action(Event *event,Queue *tellerQueue,Queue *eventQueue,int *size,int teller,float avgSerTime,float simTime,int *totSerCos,float *totSerTime,float *totSpendTime,float *totSpendTimeSqr,float *maxWaitTime,float *totIdleTime){
    if(event->id==-2){
        event->arrTime=event->time;
        int min=INT_MAX;
        int count=0;
        for(int i=0;i<teller;i++){
            if(size[i]<min){
                min=size[i];
                count=1;
            }
            else if(size[i]==min){
                count++;
            }
        }
        int index[count];
        int j=0;
        for(int i=0;i<teller;i++){
            if(size[i]==min){
                index[j]=i;
                j++;
            }
        }
        j=rand()%count;
        pushBack(&tellerQueue[index[j]],event);
        size[index[j]]++;
    }
    else if(event->id==-1){
        event->stopExe=event->time;
        totSerCos[0]++;
        totSerTime[0]=totSerTime[0]+event->stopExe-event->startExe;
        totSpendTime[0]=totSpendTime[0]+event->stopExe-event->arrTime;
        totSpendTimeSqr[0]=totSpendTimeSqr[0]+pow(event->stopExe-event->arrTime,2);
        if(event->startExe-event->arrTime>maxWaitTime[0]){
            maxWaitTime[0]=event->startExe-event->arrTime;
        }
        free(event);
    }
    else{
        if(size[event->id]>0){
            Event *event2=pop(&tellerQueue[event->id]);
            size[event->id]--;
            event2->time=event->time+(2*avgSerTime*rand()/RAND_MAX);
            event2->id=-1;
            event2->startExe=event->time;
            event->time=event2->time;
            pushTime(eventQueue,event2);
            pushTime(eventQueue,event);
        }
        else{
            bool b=true;
            for(int i=0;i<teller;i++){
                if(size[i]!=0){
                    b=false;
                }
            }
            if(b){
                float temp=(1+149.0*rand()/RAND_MAX)/60;
                event->time=event->time+temp;
                if(event->time<=simTime){
                    totIdleTime[0]=totIdleTime[0]+temp;
                }
                pushTime(eventQueue,event);
            }
            else{
                int i;
                while(true){
                    i=rand()%teller;
                    if(size[i]>0){
                        break;
                    }
                }
                Event *event2=pop(&tellerQueue[i]);
                size[i]--;
                event2->time=event->time+(2*avgSerTime*rand()/RAND_MAX);
                event2->id=-1;
                event2->startExe=event->time;
                event->time=event2->time;
                pushTime(eventQueue,event2);
                pushTime(eventQueue,event);
            }
        }
    }
}

void action2(Event *event,Queue *tellerQueue,Queue *eventQueue,int *size,float avgSerTime,float simTime,int *totSerCos,float *totSerTime,float *totSpendTime,float *totSpendTimeSqr,float *maxWaitTime,float *totIdleTime){
    if(event->id==-2){
        event->arrTime=event->time;
        pushBack(tellerQueue,event);
        size[0]++;
    }
    else if(event->id==-1){
        event->stopExe=event->time;
        totSerCos[0]++;
        totSerTime[0]=totSerTime[0]+event->stopExe-event->startExe;
        totSpendTime[0]=totSpendTime[0]+event->stopExe-event->arrTime;
        totSpendTimeSqr[0]=totSpendTimeSqr[0]+pow(event->stopExe-event->arrTime,2);
        if(event->startExe-event->arrTime>maxWaitTime[0]){
            maxWaitTime[0]=event->startExe-event->arrTime;
        }
        free(event);
    }
    else{
        if(size[0]>0){
            Event *event2=pop(tellerQueue);
            size[0]--;
            event2->time=event->time+(2*avgSerTime*rand()/RAND_MAX);
            event2->id=-1;
            event2->startExe=event->time;
            event->time=event2->time;
            pushTime(eventQueue,event2);
            pushTime(eventQueue,event);
        }
        else{
            float temp=(1+149.0*rand()/RAND_MAX)/60;
            event->time=event->time+temp;
            if(event->time<=simTime){
                totIdleTime[0]=totIdleTime[0]+temp;
            }
            pushTime(eventQueue,event);
        }
    }
}

int main(int argc,char *argv[]){
    int customer=atoi(argv[1]);
    int teller=atoi(argv[2]);
    float simTime=atof(argv[3]);
    float avgSerTime=atof(argv[4]);
    int totSerCos=0;
    float totSerTime=0.0;
    float totSpendTime=0.0;
    float totSpendTimeSqr=0.0;
    float maxWaitTime=0.0;
    float totIdleTime=0.0;
    Queue *eventQueue=(Queue*)malloc(sizeof(Queue));
    eventQueue->head=NULL;
    eventQueue->tail=NULL;
    for(int i=0;i<customer;i++){
        Event *event=(Event*)malloc(sizeof(Event));
        event->time=simTime*rand()/RAND_MAX;
        event->id=-2;
        event->arrTime=-1.0;
        event->startExe=-1.0;
        event->stopExe=-1.0;
        pushTime(eventQueue,event);
    }
    Queue tellerQueue[teller];
    int size[teller];
    for(int i=0;i<teller;i++){
        tellerQueue[i].head=NULL;
        tellerQueue[i].tail=NULL;
        size[i]=0;
        Event *event=(Event*)malloc(sizeof(Event));
        event->time=(1+599.0*rand()/RAND_MAX)/60;
        if(event->time<=simTime){
            totIdleTime=totIdleTime+event->time;
        }
        event->id=i;
        event->arrTime=-1.0;
        event->startExe=-1.0;
        event->stopExe=-1.0;
        pushTime(eventQueue,event);
    }
    while(true){
        Event *event=pop(eventQueue);
        if(event->time>simTime){
            break;
        }
        action(event,tellerQueue,eventQueue,size,teller,avgSerTime,simTime,&totSerCos,&totSerTime,&totSpendTime,&totSpendTimeSqr,&maxWaitTime,&totIdleTime);
    }
    printf("\n");
    printf("Total number of customers served: %d\n",totSerCos);
    printf("Total time required to serve all customers: %f min\n",totSerTime);
    printf("Number of tellers: %d\n",teller);
    printf("Type of queuing: one per teller\n");
    printf("Average amount of time a customer spent in the bank: %f min\n",totSpendTime/totSerCos);
    printf("Standard deviation of the amount of time a customer spent in the bank: %f min\n",sqrt((totSpendTimeSqr/totSerCos)-pow(totSpendTime/totSerCos,2)));
    printf("Maximum wait time from the time a customer arrives to the time he/she is seen by a teller: %f min\n",maxWaitTime);
    printf("Total amount of teller service time: %f min\n",totSerTime);
    printf("Total amount of teller idle time: %f min\n",totIdleTime);
    int totSerCos2=0;
    float totSerTime2=0.0;
    float totSpendTime2=0.0;
    float totSpendTimeSqr2=0.0;
    float maxWaitTime2=0.0;
    float totIdleTime2=0.0;
    Queue *eventQueue2=(Queue*)malloc(sizeof(Queue));
    eventQueue2->head=NULL;
    eventQueue2->tail=NULL;
    Queue *tellerQueue2=(Queue*)malloc(sizeof(Queue));
    tellerQueue2->head=NULL;
    tellerQueue2->tail=NULL;
    int size2=0;
    for(int i=0;i<customer;i++){
        Event *event=(Event*)malloc(sizeof(Event));
        event->time=simTime*rand()/RAND_MAX;
        event->id=-2;
        event->arrTime=-1.0;
        event->startExe=-1.0;
        event->stopExe=-1.0;
        pushTime(eventQueue2,event);
    }
    for(int i=0;i<teller;i++){
        Event *event=(Event*)malloc(sizeof(Event));
        event->time=(1+599.0*rand()/RAND_MAX)/60;
        if(event->time<=simTime){
            totIdleTime2=totIdleTime2+event->time;
        }
        event->id=i;
        event->arrTime=-1.0;
        event->startExe=-1.0;
        event->stopExe=-1.0;
        pushTime(eventQueue2,event);
    }
    while(true){
        Event *event=pop(eventQueue2);
        if(event->time>simTime){
            break;
        }
        action2(event,tellerQueue2,eventQueue2,&size2,avgSerTime,simTime,&totSerCos2,&totSerTime2,&totSpendTime2,&totSpendTimeSqr2,&maxWaitTime2,&totIdleTime2);
    }
    printf("\n");
    printf("Total number of customers served: %d\n",totSerCos2);
    printf("Total time required to serve all customers: %f min\n",totSerTime2);
    printf("Number of tellers: %d\n",teller);
    printf("Type of queuing: common\n");
    printf("Average amount of time a customer spent in the bank: %f min\n",totSpendTime2/totSerCos2);
    printf("Standard deviation of the amount of time a customer spent in the bank: %f min\n",sqrt((totSpendTimeSqr2/totSerCos2)-pow(totSpendTime2/totSerCos2,2)));
    printf("Maximum wait time from the time a customer arrives to the time he/she is seen by a teller: %f min\n",maxWaitTime2);
    printf("Total amount of teller service time: %f min\n",totSerTime2);
    printf("Total amount of teller idle time: %f min\n",totIdleTime2);
    FILE *outfile=fopen("./output/data.txt","a");
    fprintf(outfile,"%d %f\n",teller,totSpendTime2/totSerCos2);
    fclose(outfile);
    printf("\n");
    return 0;
}
