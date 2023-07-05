#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Process
{
    int pid;
    float arrivalTime;
    float burstTime;
    float burstTimeRemaining;
    float completionTime;
    float turnaroundTime;
    float waitingTime;
    float Enter; 
    float responseTime;
    bool isComplete;
    bool inQueue;
    bool in;
};

void remove(int t, queue<int> &q)
{

    queue<int> ref;
    int s = q.size();
    int cnt = 0;

    while (q.front() != t && !q.empty())
    {
        ref.push(q.front());
        q.pop();
        cnt++;
    }

    if (q.empty())
    {
        while (!ref.empty())
        {

            q.push(ref.front());
            ref.pop();
        }
    }

    else
    {
        q.pop();
        while (!ref.empty())
        {

            q.push(ref.front());
            ref.pop();
        }
        int k = s - cnt - 1;
        while (k--)
        {

            int p = q.front();
            q.pop();
            q.push(p);
        }
    }
}


void checkForNewArrivals(Process processes[], const int n, float currentTime, queue<int> &readyQueue)
{
    for (int i = 0; i < n; i++)
    {
        Process p = processes[i];

        if (p.arrivalTime <= currentTime && !p.inQueue && !p.isComplete)
        {
            processes[i].inQueue = true;
            readyQueue.push(i);
        }
    }
    // if(readyQueue.empty()){
    //     currentTime=currentTime+0.01;
    // }
}

void updateQueue(Process processes[], const int n, const float quantum, queue<int> &readyQueue, float currentTime, int programsExecuted, double avg_burstTime)
{

    while (programsExecuted != n)
    {
        int i = readyQueue.front(); 

        if (processes[i].burstTimeRemaining >= avg_burstTime)
        {
            if (!processes[i].in)
            {
                processes[i].Enter = currentTime;
                processes[i].in = true;
            }

            readyQueue.pop();

            if (processes[i].burstTimeRemaining < quantum)
            {

                processes[i].isComplete = true;
                currentTime += processes[i].burstTimeRemaining;
                processes[i].completionTime = currentTime;
                processes[i].waitingTime = processes[i].completionTime - processes[i].arrivalTime - processes[i].burstTime;
                processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstTime;
                if (processes[i].waitingTime < 0)
                    processes[i].waitingTime = 0;
                processes[i].responseTime = processes[i].Enter - processes[i].arrivalTime;
                if (processes[i].responseTime < 0)
                    processes[i].responseTime = 0;
                processes[i].burstTimeRemaining = 0;
                programsExecuted++;
                processes[i].inQueue = false;


                if (programsExecuted != n)
                {
                    checkForNewArrivals(processes, n, currentTime, readyQueue);
                }
            }

            else
            {
                processes[i].burstTimeRemaining -= quantum;
                currentTime += quantum;

                if (programsExecuted != n)
                {
                    checkForNewArrivals(processes, n, currentTime, readyQueue);
                }

                readyQueue.push(i); 
            }
        }

        else
        {
            int shortest_job = i;                                  
            float shortest_time = processes[i].burstTimeRemaining; 
            for (int i = 0; i < n; ++i) 
            {

                if (processes[i].burstTimeRemaining < avg_burstTime && processes[i].burstTimeRemaining > 0 && processes[i].burstTimeRemaining < shortest_time && processes[i].inQueue)
                {
                    shortest_job = i;
                    shortest_time = processes[i].burstTimeRemaining;
                }

                if (processes[i].burstTimeRemaining >= avg_burstTime && processes[i].burstTimeRemaining > 0 && processes[i].inQueue)
                {
                    break;
                }
            }

            if (!processes[shortest_job].in)
            {
                processes[shortest_job].Enter = currentTime;
            }

            processes[shortest_job].isComplete = true;
            currentTime += processes[shortest_job].burstTimeRemaining;
            processes[shortest_job].completionTime = currentTime;
            processes[shortest_job].waitingTime = processes[shortest_job].completionTime - processes[shortest_job].arrivalTime - processes[shortest_job].burstTime;
            if (processes[shortest_job].waitingTime < 0)
                processes[shortest_job].waitingTime = 0;
            processes[shortest_job].turnaroundTime = processes[shortest_job].waitingTime + processes[shortest_job].burstTime;
            processes[shortest_job].burstTimeRemaining = 0;
            processes[shortest_job].responseTime = processes[shortest_job].Enter - processes[shortest_job].arrivalTime;
            if (processes[shortest_job].responseTime < 0)
                processes[shortest_job].responseTime = 0;
            programsExecuted++;

            remove(shortest_job, readyQueue);

            processes[shortest_job].inQueue = false;

            if (programsExecuted != n)
            {
                checkForNewArrivals(processes, n, currentTime, readyQueue);
            }
        }
    }
}

void output(Process processes[], const int n)
{
    double avgWaitingTime = 0;
    double avgTurntaroundTime = 0;
    double avgResponseTime = 0;
    sort(processes, processes + n, [](const Process &p1, const Process &p2)
         { return p1.pid < p2.pid; });

    cout << "\n\n\t\t The Process Status \n\n";
    cout << "\tProcess ID\tArrival Time\tBurst Time\tCompletion Time\t  Turn Around Time\tWaiting Time\tResponse Time";
    for (int i = 0; i < n; ++i)
    {
        cout << "\n\t\t" << processes[i].pid << "\t\t" << processes[i].arrivalTime << "\t\t" << processes[i].burstTime << "\t\t"
             << processes[i].completionTime << "\t\t" << processes[i].turnaroundTime << "\t\t" << processes[i].waitingTime << "\t\t" << processes[i].responseTime;
        avgWaitingTime += processes[i].waitingTime;
        avgTurntaroundTime += processes[i].turnaroundTime;
        avgResponseTime += processes[i].responseTime;
    }
    avgWaitingTime /= n;
    avgTurntaroundTime /= n;
    avgResponseTime /= n;
    cout << "\n\n\t\tAverage Waiting Time: " << avgWaitingTime;
    cout << "\n\t\tAverage Turn Around Time: " << avgTurntaroundTime;
    cout << "\n\t\tAverage Response Time: " << avgResponseTime;
    cout << "\n\n\n";
}

void hybrid_scheduling(Process processes[], int n, float quantum, double avg_burstTime)
{
    queue<int> readyQueue;
    readyQueue.push(0); 
    processes[0].inQueue = true;

    float currentTime = 0;    
    int programsExecuted = 0; 

    updateQueue(processes, n, quantum, readyQueue, currentTime, programsExecuted, avg_burstTime);
}

int main()
{
    int n;

    cout << "Enter the number of processes: ";
    cin >> n;

    Process processes[n + 1];

    for (int i = 0; i < n; i++)
    {
        cout << "Enter arrival time and burst time of each process " << i + 1 << ": ";
        cin >> processes[i].arrivalTime;
        cin >> processes[i].burstTime;

        processes[i].in = false; 

        processes[i].burstTimeRemaining = processes[i].burstTime;

        processes[i].pid = i + 1;
        cout << endl;
    }

    std::sort(processes, processes + n, [](const Process &p1, const Process &p2)
              { return p1.arrivalTime < p2.arrivalTime; });

    float total_burstTime = 0;
    int num_processes = n;

    for (int i = 0; i < n; i++)
    {
        total_burstTime += processes[i].burstTime;
    }

    double avg_burstTime = static_cast<double>(total_burstTime) / num_processes;

    float sum_burstTime_RR = 0;
    float num_RR_processes = 0;

    for (int i = 0; i < n; i++)
    {
        if (processes[i].burstTime >= avg_burstTime)
        {
            sum_burstTime_RR += processes[i].burstTime;
            num_RR_processes++;
        }
    }

    float time_quantum;

    if ((sum_burstTime_RR / (num_RR_processes * num_RR_processes)) > 2)
    {
        time_quantum = sum_burstTime_RR / (num_RR_processes * num_RR_processes);
    }
    else
    {
        time_quantum = 2;
    }

    cout << endl
         << "Average burst time= " << avg_burstTime << endl;
    cout << "Time quantum= " << time_quantum << endl;

    hybrid_scheduling(processes, n, time_quantum, avg_burstTime);

    output(processes, n);

    return 0;
}