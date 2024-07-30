#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_SIZE 1000

typedef struct
{
    int pagesToPrint;
    int priority;
    int computerID;
    int JobID;
} PrintJob;

typedef struct
{
    PrintJob jobs[MAX_SIZE];
    int front;
    int rear;
} PrintQueue;

typedef struct
{
    int computerID;
    int canPrint;
    int canSetPriority;
    char password[20];
} Computer;

;
typedef struct BSTNode
{
    PrintJob job;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct
{
    BSTNode *root;
} BST;

typedef enum
{
    PRIORITY,
    FIFO,
    SJN
} SchedulingPolicy;

void initializeQueue(PrintQueue *queue)
{
    queue->front = 0;
    queue->rear = -1;
}

int isEmpty(PrintQueue *queue)
{
    return queue->rear < queue->front;
}

int isFull(PrintQueue *queue)
{
    return queue->rear == MAX_SIZE - 1;
}

void enqueue(PrintQueue *queue, PrintJob job, SchedulingPolicy policy)
{
    if (isFull(queue))
    {
        printf("Queue is full. Cannot add more jobs.\n");
        return;
    }

    if (policy == PRIORITY)
    {
        int i;
        for (i = queue->rear; i >= queue->front; i--)
        {
            if (queue->jobs[i].priority <= job.priority)
            {
                break;
            }
            queue->jobs[i + 1] = queue->jobs[i];
        }
        job.JobID = ++(queue->rear);
        queue->jobs[i + 1] = job;
        printf("Job with ID %d added successfully.\n", job.JobID);
    }
    else
    {
        job.JobID = ++(queue->rear);
        queue->jobs[queue->rear] = job;
        printf("Job with ID %d added successfully.\n", job.JobID);
    }
}

int waitTime(PrintQueue *queue, int id)
{
    int i;
    int count = 0;
    for (i = queue->front; i <= queue->rear; i++)
    {
        count += queue->jobs[i].pagesToPrint;
        if (queue->jobs[i].JobID == id)
        {
            break;
        }
    }
    if (i > queue->rear)
    {
        return -1;
    }
    return count * 5;
}

PrintJob dequeue(PrintQueue *queue)
{
    if (!isEmpty(queue))
    {
        PrintJob done = queue->jobs[(queue->front)++];
        printf("Processing Job with ID %d...\n", done.JobID);

        for (int i = 0; i < done.pagesToPrint; i++)
        {
            sleep(1);
            printf("Page %d printed.\n", i + 1);
        }

        printf("Job with ID %d finished successfully.\n", done.JobID);
        return done;
    }
    else
    {
        printf("Queue is empty.\n");
        PrintJob emptyJob = {-1, -1, -1, -1};
        return emptyJob;
    }
}

PrintJob peek(PrintQueue *queue, SchedulingPolicy policy)
{
    if (!isEmpty(queue))
    {
        PrintJob top = queue->jobs[queue->front];
        if (policy == PRIORITY)
        {
            printf("Next job in queue - Pages: %d, Priority: %d, Computer ID: %d, Job ID: %d\n",
                   top.pagesToPrint, top.priority, top.computerID, top.JobID);
        }
        else
        {
            printf("Next job in queue - Pages: %d, Computer ID: %d, Job ID: %d\n",
                   top.pagesToPrint, top.computerID, top.JobID);
        }
        return top;
    }
    else
    {
        printf("Queue is empty.\n");
        PrintJob emptyJob = {-1, -1, -1, -1};
        return emptyJob;
    }
}

void removeJobById(PrintQueue *queue, int id)
{
    int i, found = 0;
    for (i = queue->front; i <= queue->rear; i++)
    {
        if (queue->jobs[i].JobID == id)
        {
            found = 1;
            break;
        }
    }

    if (found)
    {
        for (int j = i; j < queue->rear; j++)
        {
            queue->jobs[j] = queue->jobs[j + 1];
        }
        queue->rear--;
        printf("Job with ID %d removed successfully.\n", id);
    }
    else
    {
        printf("Job with ID %d not found.\n", id);
    }
}

void printQueue(PrintQueue *queue, SchedulingPolicy policy)
{
    printf("Printing entire queue:\n");

    if (isEmpty(queue))
    {
        printf("Queue is empty.\n");
        return;
    }

    for (int i = queue->front; i <= queue->rear; i++)
    {
        PrintJob job = queue->jobs[i];
        if (policy == PRIORITY)
        {
            printf("Job ID: %d, Pages: %d, Priority: %d, Computer ID: %d\n",
                   job.JobID, job.pagesToPrint, job.priority, job.computerID);
        }
        else
        {
            printf("Job ID: %d, Pages: %d, Computer ID: %d\n",
                   job.JobID, job.pagesToPrint, job.computerID);
        }
    }
}

BSTNode *createNode(PrintJob job)
{
    BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
    newNode->job = job;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

BST *createBST()
{
    BST *tree = (BST *)malloc(sizeof(BST));
    tree->root = NULL;
    return tree;
}

BSTNode *insertSJN(BSTNode *root, PrintJob job)
{
    if (root == NULL)
    {
        return createNode(job);
    }

    if (job.pagesToPrint < root->job.pagesToPrint)
    {
        // If the new job has fewer pages, insert into the left subtree
        root->left = insertSJN(root->left, job);
    }
    else if (job.pagesToPrint > root->job.pagesToPrint)
    {
        // If the new job has more pages, insert into the right subtree
        root->right = insertSJN(root->right, job);
    }
    else
    {
        root->left = insertSJN(root->left, job);
    }

    // Return the updated root of the subtree
    return root;
}

int sjnCount = 0;
void enqueueSJN(BST *tree, PrintJob job)
{
    if (tree->root == NULL)
    {
        job.JobID = sjnCount;
        tree->root = createNode(job);
        printf("Job with ID %d added successfully.\n", job.JobID);
        sjnCount++;
    }
    else
    {
        job.JobID = sjnCount;
        tree->root = insertSJN(tree->root, job);
        printf("Job with ID %d added successfully.\n", job.JobID);
        sjnCount++;
    }
}

void inorderTraversal(BSTNode *root)
{
    if (root == NULL)
    {
        return;
    }
    inorderTraversal(root->left);
    printf("Job ID: %d, Pages: %d, Computer ID: %d\n", root->job.JobID, root->job.pagesToPrint, root->job.computerID);
    inorderTraversal(root->right);
}

void printQueueSJN(BST *tree)
{
    // Check if the tree is empty
    if (tree->root == NULL)
    {
        printf("Queue is empty.\n");
        return;
    }

    inorderTraversal(tree->root);
}

BSTNode *peekSJN(BSTNode *root)
{
    if (root == NULL)
    {
        return NULL;
    }

    if (root->left == NULL)
    {
        return root;
    }

    return peekSJN(root->left);
}

BSTNode *findJobSJN(BSTNode *root, int jobID)
{
    if (root == NULL)
    {
        return NULL;
    }

    BSTNode *leftNode = findJobSJN(root->left, jobID);
    if (leftNode != NULL)
    {
        return leftNode;
    }

    if (root->job.JobID == jobID)
    {
        return root;
    }

    BSTNode *rightNode = findJobSJN(root->right, jobID);
    if (rightNode != NULL)
    {
        return rightNode;
    }

    return NULL;
}

BSTNode *removeJobSJN(BSTNode *root, BSTNode *targetNode)
{
    if (root == NULL)
    {
        // Job not found in the tree
        return root;
    }

    if (targetNode->job.pagesToPrint < root->job.pagesToPrint)
    {
        root->left = removeJobSJN(root->left, targetNode);
    }
    else if (targetNode->job.pagesToPrint > root->job.pagesToPrint)
    {
        root->right = removeJobSJN(root->right, targetNode);
    }
    else if (targetNode->job.JobID == root->job.JobID)
    {
        // We have found the target node

        // Case 1: Leaf node
        if (root->left == NULL && root->right == NULL)
        {
            free(root);
            return NULL;
        }

        // Case 2: Node with one child
        if (root->left == NULL)
        {
            struct BSTNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            struct BSTNode *temp = root->left;
            free(root);
            return temp;
        }

        // Case 3: Node with two children
        // Find the inorder successor (smallest node in the right subtree)
        struct BSTNode *successor = root->right;
        while (successor->left != NULL)
        {
            successor = successor->left;
        }

        // Copy the successor's value to the target node
        root->job.pagesToPrint = successor->job.pagesToPrint;
        root->job.priority = successor->job.priority;
        root->job.computerID = successor->job.computerID;
        root->job.JobID = successor->job.JobID;

        // Recursively delete the successor
        root->right = removeJobSJN(root->right, successor);
    }
    else
    {
        root->left = removeJobSJN(root->left, targetNode);
    }

    return root;
}

int countPagesSJN(BSTNode *root, int jobID, int givenPages)
{
    if (root == NULL)
    {
        return 0;
    }

    int pages = 0;

    if (root->job.pagesToPrint <= givenPages)
    {
        pages += root->job.pagesToPrint;
    }

    pages += countPagesSJN(root->left, jobID, givenPages);
    pages += countPagesSJN(root->right, jobID, givenPages);

    return pages;
}

int waitTimeSJN(BST *tree, int jobID)
{
    BSTNode *node = findJobSJN(tree->root, jobID);
    if (node == NULL)
    {
        return -1; // Job not found
    }

    int pages = countPagesSJN(tree->root, jobID, node->job.pagesToPrint);
    int waitTime = pages * 5;
    return waitTime;
}

SchedulingPolicy getSchedulingPolicy()
{
    int choice;
    printf("Select the scheduling policy:\n");
    printf("1. Priority\n");
    printf("2. FIFO\n");
    printf("3. Shortest Job Next\n");
    printf("Enter your choice (1, 2, or 3): ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        return PRIORITY;
    case 2:
        return FIFO;
    case 3:
        return SJN;
    default:
        printf("Invalid choice. Defaulting to Priority.\n");
        return PRIORITY;
    }
}

Computer computers[3];

void initializeComputers()
{
    computers[0].computerID = 1;
    computers[0].canPrint = 1;
    computers[0].canSetPriority = 1;
    strcpy(computers[0].password, "1");

    computers[1].computerID = 2;
    computers[1].canPrint = 1;
    computers[1].canSetPriority = 0;
    strcpy(computers[1].password, "2");

    computers[2].computerID = 3;
    computers[2].canPrint = 0;
    computers[2].canSetPriority = 0;
    strcpy(computers[2].password, "3");
}

int authenticateComputer(int computerID, const char *password)
{
    for (int i = 0; i < 3; i++)
    {
        if (computers[i].computerID == computerID && strcmp(computers[i].password, password) == 0)
        {
            printf("Authentication successful");
            return 1;
        }
    }
    return 0; // Authentication failed
}

int main()
{
    SchedulingPolicy policy = getSchedulingPolicy();

    initializeComputers();
    PrintQueue printerQueue;
    initializeQueue(&printerQueue);

    BST *sjnTree = createBST();

    int n = 1;
    while (n)
    {
        printf("\n-------------------------------------\n");
        printf("1. Add a Print Job\n");
        printf("2. Finish a Print Job\n");
        printf("3. Peek at the Next Job\n");
        printf("4. Check Wait Time\n");
        printf("5. Remove a Print Job\n");
        printf("6. Print the Entire Queue\n");
        printf("0. Exit\n");
        printf("\n");
        printf("Enter your choice: ");
        scanf("%d", &n);

        switch (n)
        {
        case 1:;
            int computerID;
            printf("Enter the computer ID: ");
            scanf("%d", &computerID);

            int computerIndex = -1;
            for (int i = 0; i < 3; i++)
            {
                if (computers[i].computerID == computerID)
                {
                    computerIndex = i;
                    break;
                }
            }

            if (computerIndex == -1)
            {
                printf("Invalid computer ID. Try again.\n");
                continue;
            }

            int authenticationAttempts = 0;
            int authenticated = 0;

            do
            {
                char password[20];
                printf("Enter the password: ");
                scanf("%s", password);

                authenticated = authenticateComputer(computerID, password);

                if (!authenticated)
                {
                    if (2 - authenticationAttempts > 0)
                        printf("Authentication failed. Invalid password. Attempts left: %d\n", 2 - authenticationAttempts);
                    authenticationAttempts++;
                }

            } while (!authenticated && authenticationAttempts < 3);

            if (authenticated)
            {
                if (computers[computerIndex].canPrint)
                {
                    int pages, priority;
                    printf("Enter the number of pages: ");
                    scanf("%d", &pages);

                    if (computers[computerIndex].canSetPriority && policy != FIFO && policy != SJN)
                    {
                        printf("Enter the priority: ");
                        scanf("%d", &priority);
                    }
                    else
                    {
                        priority = 3;
                    }

                    int jid = printerQueue.rear + 1;
                    PrintJob newJob = {pages, priority, computerID, jid};

                    // Check the selected policy and enqueue accordingly
                    switch (policy)
                    {
                    case PRIORITY:
                        enqueue(&printerQueue, newJob, policy);
                        break;
                    case FIFO:
                        enqueue(&printerQueue, newJob, policy);
                        break;
                    case SJN:
                        enqueueSJN(sjnTree, newJob);
                        break;
                    default:
                        printf("Invalid policy.\n");
                        break;
                    }
                }
                else
                {
                    printf("Unauthorized computer. Cannot add a print job.\n");
                }
            }
            else
            {
                printf("Authentication failed.");
            }
            break;

        case 2:
            if (policy == SJN)
            {
                BSTNode *nextJob = peekSJN(sjnTree->root);

                if (nextJob != NULL)
                {
                    printf("Processing Job with ID %d...\n", nextJob->job.JobID);
                    for (int i = 0; i < nextJob->job.pagesToPrint; i++)
                    {
                        sleep(1);
                        printf("Page %d printed.\n", i + 1);
                    }
                    printf("Job with ID %d finished successfully.\n", nextJob->job.JobID);

                    // Remove the job from the print queue after printing
                    sjnTree->root = removeJobSJN(sjnTree->root, nextJob);
                }
                else
                {
                    printf("The queue is empty.\n");
                }
                break;
            }

            PrintJob done = dequeue(&printerQueue);
            break;

        case 3:;
            if (policy == SJN)
            {
                BSTNode *nextJob = peekSJN(sjnTree->root);

                if (nextJob != NULL)
                {
                    printf("Next job in the queue:\n");
                    printf("Job ID: %d, Pages: %d, Computer ID: %d\n",
                           nextJob->job.JobID, nextJob->job.pagesToPrint,
                           nextJob->job.computerID);
                }
                else
                {
                    printf("The queue is empty.\n");
                }
                break;
            }
            peek(&printerQueue, policy);
            break;

        case 4:;
            int jobid;
            int time = -1;
            printf("Enter the Job ID: ");
            scanf("%d", &jobid);
            if (policy == SJN)
            {
                time = waitTimeSJN(sjnTree, jobid);
            }
            else
            {
                time = waitTime(&printerQueue, jobid);
            }
            if (time >= 0)
            {
                printf("The wait time is %d seconds\n", time);
            }
            else
            {
                printf("Job ID not found\n");
            }
            break;

        case 5:;
            int removeId;
            printf("Enter the Job ID to remove: ");
            scanf("%d", &removeId);
            if (policy == SJN)
            {
                BSTNode *targetNode = findJobSJN(sjnTree->root, removeId);
                if (targetNode != NULL)
                {
                    sjnTree->root = removeJobSJN(sjnTree->root, targetNode);
                }
                else
                {
                    printf("Job ID not found.");
                }
                break;
            }
            removeJobById(&printerQueue, removeId);
            break;

        case 6:;

            if (policy == SJN)
            {
                printQueueSJN(sjnTree);
                break;
            }
            printQueue(&printerQueue, policy);
            break;

        case 0:
            printf("Exiting program.\n");
            break;

        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }

    return 0;
}