/* Intelligent Course Advisor */

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>

#define bool int
#define TRUE 1
#define FALSE 0
#define SUBJECT_FILENAME  "subjects.txt"

struct subject
{
	char name[8];   //course number
	int  value;     
	int  work;
};

struct dictionary
{
	struct subject *subj;
	int n;
};

void initDict(struct dictionary *pdict)
{
	pdict->subj=NULL;
	pdict->n=0;
}

void delDict(struct dictionary dict)

{
	if(dict.subj!=NULL)
            free(dict.subj);
}

struct dictionary loadSubjects(char filename[])//

{
	
	FILE *fr;
	char name[8];
	int value,work,i;
	struct dictionary dict;
	initDict(&dict);
	
	fr=fopen(filename,"r");
	if(fr!=NULL)
	{
		while(!(feof(fr)))
		{
			fscanf(fr,"%s %d %d",name,&value,&work);
			dict.n++;
		}
		rewind(fr);
		dict.subj=(struct subject*)malloc(sizeof(struct subject)*dict.n);
		i=0;
		while(!(feof(fr))&&i<dict.n)
		{
			fscanf(fr,"%s %d %d",name,&value,&work);
			strcpy(dict.subj[i].name,name);
			dict.subj[i].value=value;
			dict.subj[i].work=work;
			i++;
		}
		fclose(fr); 	
	}else{
		printf("\nFail\n");
	}
	return dict;
}

void printSubjects(struct dictionary dict)
{

	int i,totalwork=0,totalvalue=0;
	printf("Course\tValue\tWork\n");
	printf("======\t====\t=====\n");
	for(i=0;i<dict.n;i++)
	{
		printf("%s\t%d\t%d\n",dict.subj[i].name,dict.subj[i].value,dict.subj[i].work);
		totalwork+=dict.subj[i].work;
		totalvalue+=dict.subj[i].value;
	}
	printf("\n\nTotal value:%d\nTotal work:%d\n",totalvalue,totalwork);
}


//优化bool函数返回值 
bool cmpValue(struct subject sub1, struct subject sub2)
{
	if(sub1.value>sub2.value)
		return 2;
	else if(sub1.value==sub2.value)
		return 1;
	else
		return 0;
}

bool cmpWork(struct subject sub1, struct subject sub2)
{
	if(sub1.work <sub2.work)
		return 2;
	else if(sub1.work ==sub2.work)
		return 1;
	else
		return 0;
}

bool cmpRatio(struct subject sub1, struct subject sub2)
{
	float a=(float)sub1.value/(float)sub1.work/((float)sub2.value/(float)sub2.work);
	if(a>1)
		return 2;
	else if(a==1)
		return 1;
	else
		return 0;
}

struct dictionary greedyAdvisor(struct dictionary dict, int maxWork, bool (*comparator)(struct subject sub1, struct subject sub2))
{
  	//sort is more important
  	int i,j,k,flag,work;
  	struct dictionary selection;
  	initDict(&selection);
  	selection.subj=(struct subject*)malloc(sizeof(struct subject)*dict.n);
  	selection.subj[0]=dict.subj[0];
  	selection.n++;
	for(i=1;i<dict.n;i++)
  	{
  		flag=0;
  		for(j=0;j<selection.n;j++)
  		{
 			if(comparator(dict.subj[i],selection.subj[j])==2)
			{
				flag=1;
	 			for(k=selection.n;k>j;k--){
	  				selection.subj[k]=selection.subj[k-1];
	  			}
	  			selection.subj[j]=dict.subj[i];
	  			selection.n++;
				
			}else if(comparator(dict.subj[i],selection.subj[j])==1){
				if(dict.subj[i].work<selection.subj[j].work)//
				{
					flag=1;
	 				for(k=selection.n;k>j;k--){
	  					selection.subj[k]=selection.subj[k-1];
	  				}
	  				selection.subj[j]=dict.subj[i];
	  				selection.n++;
				}
				else if(dict.subj[i].work==selection.subj[j].work&&dict.subj[i].value>selection.subj[j].value)
				{
					flag=1;
	 				for(k=selection.n;k>j;k--){
	  					selection.subj[k]=selection.subj[k-1];
	  				}
	  				selection.subj[j]=dict.subj[i];
	  				selection.n++;
				}
 			}
			if(flag==1)break;
  		}
  		if(flag==0){
			selection.subj[j]=dict.subj[i];
			selection.n++;
		}
  	}
  	//test
  	//selection.n=15;
  	//printSubjects(selection);
  	
  	//select process
  	selection.n=0;
  	work=0;
  	while(work+selection.subj[selection.n].work<=maxWork){
  		work+=selection.subj[selection.n].work;
  		selection.n++;
  	}
  	//优化剩余 
  	if(work<maxWork){
  		for(i=selection.n;i<dict.n;i++){
  			if(selection.subj[i].work==maxWork-work){
  				selection.subj[selection.n]=selection.subj[i];
  				selection.n++;
  				break;
  			}
  		}
  	}
  	//printSubjects(selection);
  	return selection;
}

void bfAdvisorHelper(struct dictionary dict, int maxWork, int i, int subset[], int optset[], int *psubsetValue, int subsetWork, int tv)
{
    int j;
    if(subsetWork+dict.subj[i].work <= maxWork)
    {
        optset[i]=1;  //Assume i is chosen
        if(i<dict.n-1)
            bfAdvisorHelper(dict, maxWork, i+1, subset, optset, psubsetValue, subsetWork+dict.subj[i].work, tv);
            // Find the optimal of subproblem i+1, assumed subject i is chosen, and save the optimal value to *psubsetValue
        else
        {
            for(j=0; j<dict.n; j++)
                subset[j] = optset[j];
            *psubsetValue=tv;
        }
        optset[i]=0;  //Reset 
     }

     if(tv-dict.subj[i].value > *psubsetValue) //if not selecting subject i generates higher value than selecting i
     {
        if(i<dict.n-1)
            bfAdvisorHelper(dict, maxWork, i+1, subset, optset, psubsetValue, subsetWork, tv-dict.subj[i].value);
            //Solve subproblem i+1, not including subject i
        else
        {
            for(j=0; j<dict.n; j++)
                subset[j] = optset[j];
            *psubsetValue=tv-dict.subj[i].value;  
         }        
     }
}


struct dictionary bruteForceAdvisor(struct dictionary dict, int maxWork)
{
    int i, j=0, subsetvalue=0, tv=0;
    int *subset=malloc(dict.n*sizeof(int));
    int *optset=malloc(dict.n*sizeof(int));
    for(i=0; i<dict.n; i++)
    {
        subset[i]=0;
        optset[i]=0;
        tv += dict.subj[i].value;
    }
    bfAdvisorHelper(dict, maxWork, 0, subset, optset, &subsetvalue, 0, tv);

    struct dictionary selection;
    initDict(&selection);

    for(i=0, j=0; i<dict.n; i++)
	if(subset[i]==1) j++;
    selection.n = j;
    if(j!=0)
    {
        selection.subj=malloc(j*sizeof(struct subject));
        j=0;
        for(i=0; i<selection.n && j<dict.n; j++)
	    if (subset[j]!=0)
	       selection.subj[i++]=dict.subj[j];
    }
    return selection;
}

struct dictionary bruteForceTime(struct dictionary dict, int maxWork)
{
	time_t t1,t2;
	struct dictionary selection;
	initDict(&selection);
	time(&t1);
	selection=bruteForceAdvisor(dict,maxWork);
	time(&t2);
	printf("\nGreedy algorithm - BruteForce\nGreedy algorithm used %ld seconds to get an answer.\n",t2-t1);
	return selection;
}
/*
 Problem 3 Observations
TODO: write here your observations regarding bruteForceTime's performance:
在bruteForceTime调用bruteForceAdvisor（）函数非常的卡非常的慢，而且不知道什么时候会有结果 
*/

int dpadvisor(struct dictionary dict, int maxWork,int i,int **max){
	int without_i,with_i;
	int j;
	if(maxWork<=0||i==-1)
		return 0;
	if(max[i][maxWork]!=-1)
		return max[i][maxWork];
	without_i=dpadvisor(dict,maxWork,i-1,max);
	if(dict.subj[i].work>maxWork){
		max[i][maxWork]=without_i;
		return without_i;
	}
	else{
		with_i=dict.subj[i].value+dpadvisor(dict,maxWork-dict.subj[i].work,i-1,max);
	}
	if(without_i>with_i){
		max[i][maxWork]=without_i;
	}
	else{
		max[i][maxWork]=with_i;
	}
	return max[i][maxWork];
}

struct dictionary dpAdvisor(struct dictionary dict, int maxWork)
{
  	int i,j,**max,*subset,*set;
  	max=(int **)malloc(sizeof(int*)*dict.n);
  	set=(int *)malloc(sizeof(int)*dict.n);
  	for(i=0;i<dict.n;i++){
  		set[i]=0;
  		max[i]=(int*)malloc(sizeof(int)*(maxWork+1));
  		for(j=0;j<maxWork+1;j++)
  			max[i][j]=-1;
  	}
  	
  	struct dictionary selection;
  	initDict(&selection);
  	//get maxValue
  	dpadvisor(dict,maxWork,dict.n-1,max);
  	
	//get set
  	i=dict.n-1;j=maxWork;
  	while(i>=0&&j>0){
  		if(max[i][j]==max[i-1][j]){
  			set[i]=0;
  			i--;
  		}
  		else{
  			j=j-dict.subj[i].work;
  			set[i]=1;
  			i--;
  		}
  	}
  	for(i=0, j=0; i<dict.n; i++)
	if(set[i]==1) j++;
    selection.n = j;
    if(j!=0)
    {
        selection.subj=malloc(j*sizeof(struct subject));
        j=0;
        for(i=0; i<selection.n && j<dict.n; j++)
	    if (set[j]==1)
	       selection.subj[i++]=dict.subj[j];
    }
  	for(i=0;i<dict.n;i++){
  		free(max[i]);
  	}
  	free(max);free(subset);
  	return selection;
}

struct dictionary dpTime(struct dictionary dict, int maxWork)
{
  	time_t t1,t2;
	struct dictionary selection;
	initDict(&selection);
	time(&t1);
	selection=dpAdvisor(dict,maxWork);
	time(&t2);
	printf("\n\nDynamic Programming method used %ld seconds to get an answer.\n",t2-t1);
	return selection;
}
void greedyTime(struct dictionary dict, int maxWork)
{
	time_t t1,t2;
	
    time(&t1);
    greedyAdvisor(dict,maxWork,cmpWork);
    time(&t2);
    printf("\n\nGreedy algorithm - cmpWork\nGreedy algorithm used %ld seconds to get an answer.\n",t2-t1);
    printSubjects(greedyAdvisor(dict,maxWork,cmpWork));

 
    time(&t1);
    greedyAdvisor(dict,maxWork,cmpValue);
    time(&t2);
    printf("\n\nGreedy algorithm - cmpValue\nGreedy algorithm used %ld seconds to get an answer.\n",t2-t1);
    printSubjects(greedyAdvisor(dict,maxWork,cmpValue));
    time(&t1);
    greedyAdvisor(dict,maxWork,cmpRatio);
    time(&t2);
    printf("\n\nGreedy algorithm - cmpRatio\nGreedy algorithm used %ld seconds to get an answer.\n",t2-t1);
    printSubjects(greedyAdvisor(dict,maxWork,cmpRatio));
}


/* Problem 5 Observations
TODO: write here your observations regarding dpAdvisor's performance and
how its performance compares to that of bruteForceAdvisor： 
dpAdvisor比 bruteForceAdvisor更有效率的多 
*/


int main()
{
    struct dictionary dict;
    initDict(&dict);
    dict=loadSubjects(SUBJECT_FILENAME);
	int maxWork;
	printf("\nPlease enter your total work hours:");
	scanf("%d",&maxWork);fflush(stdin);
	printf("\n\nThe recommended list are:\n\n");
	greedyTime(dict,maxWork);
    printSubjects(dpTime(dict,maxWork));
    delDict(dict);
    system("pause");
}



