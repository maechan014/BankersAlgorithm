#include <stdio.h>
#include <iostream>
#include <stdlib.h>
/**
    Banker's Algorithm
    In this simulator, the processes and resources starts from 1 instead of 0 (P[1], P[2], P[3], ...) instead of (P[0], P[1], P[2] ...)

    @author Charie Mae
**/

using namespace std;

/* This class is used to simulate the deadlock prevention for Banker's Algorithm with a single resource.*/
class DeadlockPrevention{
public:
    int process_size;
    int available_rsrc;
    int work_rsrc;
    int *process;   /* holds the allocation value of each processes */
    int *safe;      /* holds the process that was successfully executed. This is to be used later in printing the safe sequence*/
    bool *finish;   /* holds the list of processes that were already visited. Mainly used in if-statements*/

    DeadlockPrevention(){
        process_size = 0;
        available_rsrc = 0;
        work_rsrc = available_rsrc;
    }

    void init (){
    /* ask inputs for the process size and the value of available resource*/
        cout << "--DEADLOCK PREVENTION--" << endl;
        cout << "Enter number of processes: ";
        cin >> process_size;
        cout << "Enter available value for a single resource: ";
        cin >> available_rsrc;
        cout << endl;

        process = new int[process_size];
        finish = new bool[process_size];
        safe = new int[process_size];
    }


    void setAllocation(){
    /* ask inputs for the resource allocation of each processes */
        for (int i = 0; i < process_size; i++){
            cout << "Resource allocation for  P[" << (i+1) << "]: ";
            cin >> process[i];
        }
    }

    void run(){
    /* Run the simulator */
        int idx = 0, loop = 0;
        for(; loop < process_size; loop++){
            for (int i = 0; i < process_size; i++){
                if (process[i] <= available_rsrc && !finish[i]){
                    safe[idx] = i+1;
                    available_rsrc += process[i];
                    finish[i] = true;
                    idx += 1;
                }
            }
        }
        /* after checking all the processes, this function is called to check if the process is safe or deadlock*/
        checkState(loop);
    }

    bool visited (){
        /* Returns true if all processes were successfully executed. Returns false, otherwise**/
        int done = 0;
        for (int i = 0; i < process_size; i++){
            if(finish[i]){
                done += 1;
            }
        }
        if (done == process_size){
            return true;
        } else {
            return false;
        }
    }

    void checkState(int loop){
        if (loop == process_size && visited()){
            cout << endl;
            cout << "System State: SAFE" << endl;
            printSafeSeq();
        } else if (!visited()){
            /*The process is in deadlock when some of the visited processes are false. Meaning, the process is unfinished and may cause a deadlock*/
            cout << endl;
            cout << "System State: Deadlock!" << endl;
        }
    }

        void printSafeSeq(){
        cout << "Safe Sequence: [";
            for (int i = 0; i < process_size; i++){
                cout << safe[i];
                if (i+1 != process_size){
                  cout << ",";
                }
            }
        cout << "]" << endl;
    }

};

/* This class is used to simulate the deadlock prevention for Banker's Algorithm with multiple resources. */
class DeadlockAvoidance{
public:
    DeadlockPrevention A;
    int rsrc_size;
    int *available;
    /* The value below are 2-D arrays that holds the values of each allocation per matrix*/
    int *rsrc_alloc;
    int *max_demand;
    int *needed_alloc;

    DeadlockAvoidance(){
        rsrc_size = 0;
    }
    /* ask inputs for the number of processes and the number of resources */
    void init(){
        cout << "---DEADLOCK AVOIDANCE---" << endl;
        cout << "Enter number of processes: ";
        cin >> A.process_size;
        cout << "Enter number of resources: ";
        cin >> rsrc_size;

        A.process = new int[A.process_size];
        A.finish = new bool[A.process_size];
        A.safe = new int[A.process_size];
        available = new int[rsrc_size];
        rsrc_alloc = (int *)malloc(A.process_size * rsrc_size * sizeof(int));
        max_demand = (int *)malloc(A.process_size * rsrc_size * sizeof(int));
        needed_alloc = (int *)malloc(A.process_size * rsrc_size * sizeof(int));
    }

    void setAvailable(){
        cout << endl << "*** SET AVAILABLE RESOURCES" << endl;
        for (int i = 0; i < rsrc_size; i++){
            cout << "- Resource [" << (i+1) << "]: ";
            cin >> available[i];
        }
        cout << endl;
    }

    void setAllocation(){
        cout << endl << "*** SET RESOURCE ALLOCATION" << endl;
        for(int i = 0; i < A.process_size ; i++){
            cout << endl << "PROCESS " << i+1 << endl;
            for(int j = 0; j < rsrc_size; j++){
                cout << "- Resource[" << j+1 << "]: ";
                cin >> *(rsrc_alloc + i*rsrc_size + j);
            }
        }
    }

    void setMaximumDemand(){
        cout << endl << "***SET MAXIMUM DEMAND" << endl;
        for(int i = 0; i < A.process_size ; i++){
            cout << endl << "PROCESS " << i+1 << endl;
            for(int j = 0; j < rsrc_size; j++){
                cout << "- Resource[" << j+1 << "]: ";
                cin >> *(max_demand + i*rsrc_size + j);
            }
        }
    }

    void setNeededAlloc(){
    /* To get the needed allocation, we need to subtract the resource allocation from the maximum demand*/
        for(int i = 0; i < A.process_size ; i++){
            for(int j = 0; j < rsrc_size; j++){
                *(needed_alloc + i*rsrc_size + j) = *(max_demand + i*rsrc_size + j) - *(rsrc_alloc + i*rsrc_size + j) ;
            }
        }
    }

    /* Accepts a DeadlockPrevention as a parameter and prints the matrix of either Maximum Demand, Needed Allocation, or Resource Allocation by calling the object*/
    void drawMatrix(int *D){
        for(int i = 0; i < A.process_size; i++){
            cout << endl << "P" << i+1 << " -";
            for(int j = 0; j < rsrc_size; j++){
                cout << " " << *(D + i*rsrc_size + j);
            }
        }
        cout << endl;
    }

    /* Since the available array is a single array, the printing of its matrix is implemented differently*/
    void drawAvailableMatrix(){
        for(int j = 0; j < rsrc_size; j++){
            cout << available[j] << " ";
        }
        cout << endl;
    }

    /*Run Banker's algorithm*/
    void run(){
        int idx = 0, loop = 0, j, temp;
        int work [rsrc_size];
        int one = 1;

        for(int i = 0; i < rsrc_size; i++){
        /* This loop assigns the value of available resource to the working value. This is to avoid  the value of available resource from changing */
            work[i] = available[i];
        }

        for(; loop < A.process_size; loop++){
        /* this loop checks if the needed allocation can be accommodated by the working value/allocation */
            for (int i = 0; i < A.process_size; i++){
                for (j = 0, temp = j; temp < rsrc_size;){
                    /*  Checks if every value of in the needed allocation of the resources in each process can be accommodated in the working allocation*/
                    if (*(needed_alloc + i*rsrc_size + temp) <= work[temp] && !A.finish[i]){
                        temp++;
                    }
                    else {
                        j++;
                        break;
                    }
                }
                if (temp == rsrc_size){
                     A.safe[idx] = i+1;
                    for (int c = 0; c < rsrc_size; c++){
                        work[c] += *(needed_alloc + i*rsrc_size + c);
                    }
                    A.finish[i] = true;
                    idx += 1;
                }
            }
        }
        A.checkState(loop);
    }

};

void makeRequest(DeadlockAvoidance);
void drawMatrices(DeadlockAvoidance);
void resetFinish(DeadlockAvoidance);
void clr();
int callMenu();

int main(){
    DeadlockPrevention p;
    DeadlockAvoidance a;
    int menu;
    int choice = 0, request;

    while (choice != 3){
        menu = 0;
        choice = callMenu();

        if (choice == 1){
            p.init();
            p.setAllocation();
            p.run();
            clr();

        } else if (choice ==2){
            a.init();
            a.setAllocation();
            a.setMaximumDemand();
            a.setAvailable();
            a.setNeededAlloc();
            drawMatrices(a);
            a.run();

            cout << endl <<"-----------------------------" << endl;
            cout << "Do you want to make a request?" << endl;

            do {
                cout << "[1] Yes" << endl << "[2] No" << endl;
                cout << endl << "--> ";
                cin >> request;

                if (request == 1){
                    makeRequest(a);
                    cout << endl << "Make another request?" << endl;
                } else if (request == 2){
                    cout << endl << "No request was made." << endl;
                    clr();

                }
            } while (request != 2);
        }
    }
return 0;
}

// FUNCTIONS

int callMenu(){
    int choice;

    cout << "[1] Deadlock Prevention" << endl;
    cout << "[2] Deadlock Avoidance" << endl;
    cout << "[3] Exit program" << endl;
    cout << endl;
    cout << "CHOICE: ";
    cin >> choice;
    cout << endl;

    return choice;
}

void clr(){
    int menu;

    cout << endl << "Press 1 then ENTER to return to Main Menu." << endl;
    cin >> menu;

    if (menu == 1){
        system("CLS");
    }
}

void drawMatrices(DeadlockAvoidance a){
     cout <<"--------------------------" << endl << "RESOURCE ALLOCATION MATRIX" << endl;
    a.drawMatrix(a.rsrc_alloc);
    cout <<"--------------------------" << endl << "MAXIMUM DEMAND MATRIX" << endl;
    a.drawMatrix(a.max_demand);
    cout <<"--------------------------" << endl << "NEEDED ALLOCATION MATRIX" << endl;
    a.drawMatrix(a.needed_alloc);
    cout <<"--------------------------" << endl << "AVAILABLE ALLOCATION MATRIX" << endl;
    a.drawAvailableMatrix();

}

void resetFinish(DeadlockAvoidance a){
    int p_size = a.A.process_size;
    for (int i = 0; i < p_size; i++){
        a.A.finish[i] = false;
    }
}

void makeRequest(DeadlockAvoidance d){
    int p_size = d.A.process_size;
    int r_size = d.rsrc_size;
    int process_id;
    int *new_resource = new int [r_size], *temp = new int[r_size];

    /*
        in the DeadlockPrevention class, there's a finish array that stores the successfully executed processes.
        This resetFinish() function is to set all elements in the array to false again so that it could be use in
        the request that the user makes
    */
    resetFinish(d);


    cout << endl << "Enter process number [1-" << p_size << "]: ";
    cin >> process_id;
    cout << "Enter the request for the ff: " << endl;

    for (int i = 0; i < r_size; i++){
        cout << "- Resource[" << i+1 << "]: ";
        cin >> new_resource[i];
    }

    /* add the new resource request to the existing resource in the current process */
    for (int j = 0; j < r_size; j++){
        int t = d.available[j];
        temp[j] = *(d.rsrc_alloc + (process_id-1)*d.rsrc_size + j);
        *(d.rsrc_alloc + (process_id-1)*d.rsrc_size + j) += new_resource[j];
        d.available[j] -= new_resource[j];

        /* checks if available resource is still greater than the allocated resource */
        if (d.available[j] < temp[j]){
            cout << "Not enough available resources." << endl;
            break;
        }
    }
    cout << endl << "NEW MATRICES" << endl << endl;

    d.setNeededAlloc(); /* set new needed allocation */
    drawMatrices(d);    /* draw the new matrices */
    d.run();            /* run the Banker's Algorithm again */
}
