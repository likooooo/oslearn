#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/* Process Status */
#define PROCESS_STA_RUNNING 	'R'
#define PROCESS_STA_SLEEPING 	'S'
#define PROCESS_STA_WAITING 	'D'
#define PROCESS_STA_ZOMBIE 	'Z'
#define PROCESS_STA_STOPPED 	'T'
#define PROCESS_STA_TRACING 	't'
#define PROCESS_STA_DEAD 	'X'

#define PROCESS_MAX_COUNT 	1024 //((unsigned int)(-1)) 
#define PROCESS_NAME_MAX_SIZE 	1024 
#define PSTREE_INVALIDE_VALUE	-1

#ifdef __DEBUG
#	define  DISPLAY_STA(sta) 	\
		printf			\
		(			\
		 	"pid 	: %u\n"	\
			"comm 	: %s\n"	\
			"state 	: %c\n"	\
			"ppid 	: %u\n"	\
			"pgrp 	: %u\n"	\
			"session : %u\n"	\
			"tty nr  : %u\n",\
			sta->pid,	\
			sta->comm,	\
			sta->state,	\
			sta->ppid,	\
			sta->pgrp,	\
			sta->session,	\
			sta->tty_nr	\
		)
#	define DISPLAY_STA_TASKS(task)		\
	for(int i = 0; i < task->count; i++)	\
		printf("  thread : %d\n", task->pids[i]);
#else
#	define DISPLAY_STA(sta)
#	define DISPLAY_STA_TASKS(task)
#endif

#define PTR_SIZE (sizeof(void*)) 
#define TREE_SPLIT "--"
#define INVALID_PID 0

void* ForceMalloc(int n)
{
	void* p = NULL;
	int tryCount = 0;
	while( NULL == p && 10 > tryCount )
	{
		p = malloc(n);
	}
#ifdef __DEBUG
	if(NULL == p)
	{
		printf("out of memory\n");
	}
#endif
	return p;
}

/* /Proc/[pid]/task/ */
struct Task
{
	pid_t	pids[ PROCESS_MAX_COUNT ];
	int	count;
};

/* /Proc/[pid]/stat */
typedef struct Stat** StatList;
struct Stat
{
	pid_t pid;
	char comm[ PROCESS_NAME_MAX_SIZE ];
	char state;
	pid_t ppid;
	pid_t pgrp;
	pid_t session;
	pid_t tty_nr;

	struct Task 	task; 
	struct Stat* 	parent;
	StatList 	children;
	int 	 	childrenCount;
	
	int		printLocation;
};

/* 获取指定进程 id 的所有线程 */
int GetProcessTasks(pid_t pid, struct Task* task)
{
	char strPath[255];
	sprintf(strPath, "/proc/%d/task", pid);
	DIR* dir = opendir(strPath);
	if(NULL == dir)
		return -1;
	struct dirent* file;
	task->count = 0;
	while( file = readdir( dir ) )
	{
		task->pids[ task->count ] = atoi(file->d_name);
		if( 0 < task->pids[ task->count ] )
		{
		 	task->count++;
		}
	}
	DISPLAY_STA_TASKS(task);
	return task->count;
}
/* 获取指定进程 id 的进程信息 */
int GetProcessStat(pid_t pid, struct Stat* sta)
{
	char strPath[255];			
	sprintf(strPath, "/proc/%d/stat", pid);	
	FILE* fp = fopen(strPath, "r");		
	if(0 == fp) 
		return -1;			
	fscanf( fp, "%d %s %c %d %d %d %d", 
		&( sta->pid 	),
		sta->comm	,
		&( sta->state 	),
		&( sta->ppid 	),
		&( sta->pgrp 	),
		&( sta->session ),
		&( sta->tty_nr 	)
	);
	int len = strlen(sta->comm);
	for(int i = 0; i < len - 2; i++)
	{
		sta->comm[i] = sta->comm[i + 1];
	}
	sta->comm[len - 2] = 0;
	DISPLAY_STA(sta);	
	fclose(fp);
	fp = NULL;
	return 0;					
}
/* 获取当前系统中所有的进程 id */
int GetAllProcessPid(int* pids)
{
	DIR* dir = opendir("/proc");
	if(NULL == dir)
		return -1;
	struct dirent* file;
	int count = 0;
	while( file = readdir( dir ) )
	{
		pids[count] = atoi(file->d_name);
		if( 0 < pids[count] )
		{
		 	count++;
		}
	}
	return count;
}
/* 查找进程信息中指定 pid 对应的进程详情 */
int FindStatByPid(pid_t pid, struct Stat* list, int count)
{
	for(int i = 0; i < count; i++)
	{
		if(pid == list[i].pid)
		{
			return i; 
		}
	}
#ifdef __DEBUG
	printf("Find stat by fid failed pid : %d\n", pid);
#endif
	return -1;
}
/* 创建树信息 */
struct Stat* /* root*/ MakeTree(struct Stat* ps, int count)
{
	struct Stat* root = NULL;
	for(int i = 0; i < count; i++)
	{
		ps[i].childrenCount = 0;
		if(1 == ps[i].pid)
		{
			ps[i].parent = NULL;
			root = ps + i;
			continue;
		}
		else
		{
			int index = FindStatByPid(ps[i].ppid, ps, count);
			if(-1 == index)
			{
				return NULL;
			}	
			ps[i].parent = ps + index; 
		}
		if(NULL == ps[i].parent->children)
			ps[i].parent->children = (StatList)ForceMalloc(PTR_SIZE * count);
		ps[i].parent->children[ps[i].parent->childrenCount] = ps + i;
	       	ps[i].parent->childrenCount++;	
	}
	
#ifdef __DEBUG
	for(int i = 0; i < count; i++)
	{
		printf("%s(%u)\n", ps[i].comm, ps[i].pid);
		for(int j = 0; j < ps[i].childrenCount; j++)
		{
			printf("    %s(%u)\n", ps[i].children[j]->comm, ps[i].children[j]->pid);
		}
	}
#endif
	return root;
}
/* 释放所有树信息占用的资源 */
void ReleaseTree(struct Stat* ps, int count)
{
	for(int i = 0; i < count; i++)
	{
		if(ps[i].childrenCount)
		{
#ifdef __DEBUG
			if(NULL == ps[i].children)
			{
				printf("memory leaked at ps - %d\n", i);
			}
#endif
			free(ps[i].children);
		}
	}
}
int (*ProcessNodePrint)(struct Stat*);
int (*ThreadNodePrint)(struct Stat*, int taskIndex);
/* pstree */
int PrintStat(struct Stat* sta)
{
	return printf("%s", sta->comm);
}
/* pstree -p */
int PrintStatWithPid(struct Stat* sta)
{
	return printf("%s(%u)", sta->comm, sta->pid);
}
int PrintTask(struct Stat* sta, int taskIndex)
{
	return printf("{%s}", sta->comm);
}
int PrintTaskWithPid(struct Stat* sta, int taskIndex)
{
	return printf("{%s}(%u)", sta->comm, sta->task.pids[taskIndex]);
}
#ifdef __DEBUG
int PrintDebug(struct Stat* sta)
{
	return printf("%s(%u)", sta->comm, sta->pid);
}
#endif

void _PrintAllNode(struct Stat* root, int depth, int* isNeedFill, int* bufferLen)
{
	if(*isNeedFill)	/* 上一次递归包含换行操作, 需要竖直方向对齐 */
	{
		for(int i = 0; i < root->parent->printLocation - 1; i++)
		{
			printf(" ");
		}
		(*bufferLen)	= root->parent->printLocation;
		(*isNeedFill)	= 0;
		(*bufferLen) 	+= printf("|");
	}

	/* 打印节点信息, 并记录当前节点起始打印位置 */
	(*bufferLen) += printf(TREE_SPLIT);
	(*bufferLen) += (*ProcessNodePrint)(root);
	root->printLocation = (*bufferLen);
	
	/* 打印子节点和子线程 */
	for(int i = 0; i < root->childrenCount; i++)
	{
		_PrintAllNode(root->children[i], depth + 1, isNeedFill, bufferLen);
	}
	for(int i = 1; i < root->task.count; i++)
	{
		for(int i = 0; i < root->printLocation - 1; i++)
		{
			printf(" ");
		}
		printf("|" TREE_SPLIT);
		(*ThreadNodePrint)(root, i);
		printf("\n");
	}

	/* 如果进程没有子节点代表需要换行*/
	if(0 == root->childrenCount)
	{
		printf("\n");
		(*isNeedFill) = 1;
	}
}
void ProcCmd(int argc, char** argv, struct Stat* root)
{
	ProcessNodePrint = &PrintStat;
#ifdef __DEBUG
	for(int i = 0; i < argc; i++)
	{
		printf("\"%s\"\n", argv[i]);
	}
#endif
	ProcessNodePrint = &PrintStat;
	ThreadNodePrint = &PrintTask;
	if(1 < argc && 0 == strcmp(argv[1], "-p"))
	{
		ProcessNodePrint = &PrintStatWithPid;
		ThreadNodePrint = &PrintTaskWithPid;
	}
#ifdef __DEBUG
	ProcessNodePrint = &PrintDebug;
#endif
	int isNeedFill = 0;
	int bufferLen = 0;
	_PrintAllNode(root, 0, &isNeedFill, &bufferLen);
}
int main(int argc, char** argv)
{
	int pids[ PROCESS_MAX_COUNT ];
	int pidsCount 	= GetAllProcessPid(pids);
	int psCount 	= 0;
	struct Stat* ps = (struct Stat*)ForceMalloc(sizeof(struct Stat) * pidsCount);
	for( int i = 0; i < pidsCount; i++ )
	{
		if ( ( 0 == GetProcessStat(pids[i], ps + psCount) ) &&
		     ( GetProcessTasks(ps[psCount].pid, &ps[psCount].task)))
		{
#ifdef __DEBUG
			printf("Get Process info sucess\n");
#endif
			psCount++;
		}
		else
		{
#ifdef __DEBUG
			printf("Get Process info failed\n");
#endif
		}
	}
	struct Stat* root;
	if(root = MakeTree(ps, psCount))
	{
#ifdef __DEBUG
		printf("Make tree sucess\n");
#endif
	}
	else
	{
#ifdef __DEBUG
		printf("Make tree failed\n");
#endif
		return 2;
	}

	ProcCmd(argc, argv, root);
	ReleaseTree(ps, psCount);
	free(ps);
	return 0;
}
