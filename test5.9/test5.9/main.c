//
//  main.c
//  test5.9
//
//  Created by kira on 2019/5/9.
//  Copyright © 2019年 kira. All rights reserved.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //Linux下sleep（）头函数
#include <math.h>
#include <termios.h>//linux下写getch函数所需的头文件
#include <assert.h>
#define Start 0
#define Check 1
#define Change 2
#define Calculate 3
#define Exit 4
#define Login 10
#define Add 11
#define Delete 12
#define Income 13
#define Date  14
#define YearlyIncome 15
#define MonthlyIncome 16
#define SignUp 21
#define Initial 22
#define Password 23
#define TIME 600000

/*结构体*/
typedef struct _info
{   char name[20]; //名字
    char income[20]; //收入金额
    char date[40];//日期
    char code[40];//密码
    struct _info *next;
}Information;

/*Linux下没有的功能性函数*/
void gotoxy(int x,int y); //光标定位
int mygetch(void); //getch()
int getpasswd(char *passwd, int size);//输入字符对屏幕输出“*“
int Trans_StrToNum(char *p);//将字符串转化为数字

/*各个菜单的函数声明*/
int Login_Page(void);
void Start_Page(void);
void Check_Income_Page(void);
int Add_Record(void);
Information* Create_List(void);
void AddRecord_Page(void);
void Change_Record_Page(void);
void Calculate_Income_Page(void);
void Exit_Page(void);
Information * Delete_Record(Information *first);
void Write_Back(Information *first);
int DeleteRecord_Page(void);
Information* Select_LateDate(Information *p,Information *max);
Information* Select_LargeIncome(Information *p);
int Checkbydate_Page(void);
int Checkbyincome_Page(void);
Information* Sort_Descending(Information* first);//日期降序
int MonthlyIncome_Page(void);
int YearlyIncome_Page(void);
int Separate_Date(char*p,int choice);//choice为0分离年，choice为1分离月
void Initial_Page(void);
int SignUp_Page(void);
int ChangePassword_Page(void);

int num=Initial;//菜单数字
Information user;//登陆的用户

int main()
{
    system("clear");
    while(1){
        switch(num){
            case Start:
                system("clear");Start_Page();break;
            case Check:
                system("clear");Check_Income_Page();break;
            case Change:
                system("clear");Change_Record_Page();break;
            case Calculate:
                system("clear");Calculate_Income_Page();break;
            case Exit:
                system("clear");Exit_Page();break;
            case Login:
                system("clear");Login_Page();break;
            case Add:
                system("clear");AddRecord_Page();break;
            case Delete:
                system("clear");DeleteRecord_Page();break;
            case Income:
                system("clear");Checkbyincome_Page();break;
            case Date:
                system("clear");Checkbydate_Page();break;
            case YearlyIncome:
                system("clear");YearlyIncome_Page();break;
            case MonthlyIncome:
                system("clear");MonthlyIncome_Page();break;
            case SignUp:
                system("clear");SignUp_Page();break;
            case Initial:
                system("clear");Initial_Page();break;
            case Password:
                system("clear");ChangePassword_Page();break;
            default:
                exit(0);
                break;
        }
    }
}

void gotoxy(int x,int y)   //Fantasy
{
    printf("%c[%d;%df",0x1B,y,x);
}

int mygetch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int getpasswd(char *passwd, int size)
{
    int c, n = 0;
    do{
        c = mygetch();
        if (c != '\n' && c != '\r' && c != 127){
            passwd[n] = c;
            printf("*");
            n++;
        }
        else if ((c != '\n' | c != '\r') && c == 127){//判断是否是回车否则退格
            if (n > 0){
                n--;
                printf("\b \b");//输出退格
            }
        }
    }while (c != '\n' && c != '\r' && n < (size - 1));
    passwd[n] = '\0';//消除一个多余的回车
    return n;
}

void Start_Page(){
    printf("*******************************************\n\n\n");
    printf("        1.Check Your Income Records\n\n");
    printf("        2.Change Records\n\n");
    printf("        3.Calculate Personal Income\n\n");
    printf("        4.Exit\n\n\n");
    printf("*******************************************\n");
    printf("Choose The Function Number:");
    scanf("%d",&num);
}

int Add_Record(){    //增加记录
    FILE *fp;
    char s_income[20];
    char s_date[40];
    char *p;
    Information last; //为了停止符#不被保存在全局变量中！！
    fp=fopen("info.txt", "a+t");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    fprintf(stdout, "Please Enter Your Income(Enter '#' To Return):");
    fscanf(stdin,"%s",user.income);
    strcpy(s_income, user.income);
    for(p=s_income;*p;p++){        //输入#表示停止录入数据
        if(*p=='#'){
            strcpy(user.income, last.income);
            fclose(fp);
            system("clear");
            return num=Change;
        }
    }
    fprintf(stdout, "\nPlease Enter The Date(Enter '#' To Return):");
    fscanf(stdin,"%s",user.date);
    fprintf(stdout,"\n");
    strcpy(s_date, user.date);
    for(p=s_date;*p;p++){
        if(*p=='#'){
            strcpy(user.date, last.date);
            fclose(fp);
            return num=Change;
        }
    }
    fwrite(&user, sizeof(Information), 1, fp);//fwrite后面必须跟fclose,否则就指向下一个进行数据读写的位置 fwrite和fread可以直接读取结构体据
    strcpy(last.income, user.income);
    strcpy(last.date, user.date);
    fclose(fp);
    return num=Add;
}

Information* Create_List(){  //建立链表
    Information *head,*p,*q;//p指向新申请的节点，q指向末尾节点
    Information t;
    FILE *fp;
    head=(Information *)malloc(sizeof(Information));//给head分配内存
    p=q=head;
    q->next=NULL;
    fp=fopen("info.txt", "r");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    while (fread(&t, sizeof(Information), 1, fp)!=0) {
        p=(Information *)malloc(sizeof(Information));
        strcpy(p->name,t.name);
        strcpy(p->income,t.income);
        strcpy(p->date,t.date);
        strcpy(p->code,t.code);
        q->next=p;//首结点未写入数据！！！
        q=q->next;
    }
    q->next=NULL;
    fclose(fp);
    
    return head;
}

void AddRecord_Page(){
    Add_Record();
    Create_List();
}

void Change_Record_Page(){
    int x;
    printf("*******************************************\n\n\n\n");
    printf("        1.Add New Record\n\n");
    printf("        2.Delete Existing Record\n\n");
    printf("        3.Back\n");
    printf("\n\n\n*******************************************\n");
    printf("Please Enter Your Choice:");
    scanf("%d",&x);
    switch(x){
        case 1: num=Add;break;
        case 2: num=Delete;break;
        case 3: num=Start;break;
        default: exit(0);break;
    }
}
void Exit_Page(){
    printf("*******************************************\n\n\n\n\n\n");
    printf("        THANK U FOR USING :)\n\n\n\n\n\n");
    printf("*******************************************\n");
    exit(0);
}

Information * Delete_Record(Information *first){
    //首结点无数据!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Information *p,*q,*t;//q为p的前驱结点
    Information delete;
    p=first->next;
    q=first;//q是前驱结点需要指向first而不是first->next!!!不然会漏掉第一个结点
    printf("Please Enter The Record You Want to Delete (Format:Income Date):\n");
    scanf("%s %s",delete.income,delete.date);
    if(first==NULL){//空链表
        printf("No Valid Data Exist");
        return NULL;
    }
    //检测输入的数据是否存在于现有链表中
    t=first->next;
    while(t!=NULL){
        if((!strcmp(t->income, delete.income))&&(!strcmp(t->date, delete.date))){
            break;
        }
        else{
            t=t->next;
            continue;
        }
    }
    if(t==NULL){
        printf("The Data You Input is NOT VALID, Please Try Again\n");
        return first;
    }
    //在链表中删除该结点
    for(q=first;q->next;q=q->next){
        p=q->next;
        if((!strcmp(p->income, delete.income))&&(!strcmp(p->date, delete.date))){
            q->next=p->next;
            free(p);
            break;
        }
    }
    printf("Success!\n");
    /*
     t=first->next;
     while(t!=NULL){
     printf("%s %s\n",t->income,t->date); //用for（；t->next;t=t->next）循环会漏掉最后一个
     t=t->next;
     }
     num=Change;
     */
    return first;
}

void Write_Back(Information *first){
    FILE *fp;
    Information *a=first->next;
    fp=fopen("info.txt", "w+"); //重新写入数据
    while(a!=NULL){ //首结点无数据！！！
        strcpy(user.name, a->name);
        strcpy(user.income, a->income);
        strcpy(user.date, a->date);
        strcpy(user.code, a->code);
        fwrite(&user, sizeof(Information), 1, fp);
        a=a->next;
    }
    fclose(fp);
}

int DeleteRecord_Page(){
    Information *t,*a;
    a=Create_List();
    t=Delete_Record(a);
    //检测此时文件是否为空
    FILE *fp;
    char ch;
    fp=fopen("info.txt", "r");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    ch=fgetc(fp);
    if(ch==EOF){
        system("clear");
        printf("*******************************************\n\n\n\n\n");
        printf("        The File Is Empty:(!");
        printf("\n\n\n\n\n\n\n*******************************************\n");
        sleep(3);//延时3s
        return num=Change;
    }
    Write_Back(t);
    //判断是否继续
    char j;
    printf("Continue This Process?(Y/N):");
    scanf("%c",&j);
    if(getchar()=='N'){
        return num=Change;
    }
    return num=Delete;
}

Information* Select_LateDate(Information *p,Information *q){
    Information *max=q;
    int y,ym,m,mm,d,dm;
        y=Separate_Date(p->date, 0);//对年进行比较
        ym=Separate_Date(q->date, 0);
        if(y==ym){
            m=Separate_Date(p->date, 1);//对月进行比较
            mm=Separate_Date(q->date, 1);
            if(m==mm){
                d=Separate_Date(p->date, 2);//对日进行比较
                dm=Separate_Date(q->date,2);
                if(d==dm||d>dm){max=p;}
            }
            if(m>mm){max=p;}
        }
        if(y>ym){max=p;}
    return max;
}

Information* Sort_Descending(Information* first){
    Information *p,*q,*late,*t,*s,*c;
    int cr;
    q=first->next;
    p=q->next;
    t=(Information*)malloc(sizeof(Information));
    while(p!=NULL){
        s=p;
        late=s;
        while(s!=NULL){
           late=Select_LateDate(s,late);
            s=s->next;
        }
        c=Select_LateDate(late, q);
        cr=strcmp(c->date, q->date);
        if(cr!=0){
            strcpy(t->name,q->name);
            strcpy(t->income,q->income);
            strcpy(t->date,q->date);
            strcpy(t->code, q->code);
            strcpy(q->name,late->name);
            strcpy(q->income,late->income);
            strcpy(q->date,late->date);
            strcpy(q->code, late->code);
            strcpy(late->name,t->name);
            strcpy(late->income,t->income);
            strcpy(late->date,t->date);
            strcpy(late->code, t->code);
        }
        q=q->next;
        p=p->next;
    }
    free(t);
    return first;
}

int Checkbydate_Page(){
    Information *p,*first;
    char x;
    first=Create_List();
    first=Sort_Descending(first);
    p=first->next;
    printf("*******************************************\n\n");
    while(p!=NULL){
        if(!strcmp(p->name,user.name)){
            printf("          %s    %-6s %-6s\n",p->name,p->income,p->date);
        }
        p=p->next;
    }
    printf("\n*******************************************\n");
    printf("Enter '#' To Return:");

    scanf("%c",&x);
    if(getchar()=='#'){
        return num=Check;
    }
    return num=Date;
}

int Trans_StrToNum(char *p){
    int i=0,n=0,sum=0;
    while(p[n]!='\0'){
        n++;
    }
    for(i=0;i<n;i++){
        sum=sum+(p[i]-48)*pow(10,n-i-1);
    }
    return sum;
}

Information* Select_LargeIncome(Information *p){
    Information *max=p;
    int nummax,num0;
    while(p!=NULL){
        nummax=Trans_StrToNum(max->income);
        num0=Trans_StrToNum(p->income);
        if(num0>nummax){
            max=p;
        }
        p=p->next;
    }
    return max;
}

int Checkbyincome_Page(){
    Information *p,*first,*q,*large,*t;
    int numlarge,numq; char x;
    first=Create_List();
    q=first->next;
    p=q->next;
    t=(Information*)malloc(sizeof(Information));
    while(p!=NULL){
        large=Select_LargeIncome(p);
        numq=Trans_StrToNum(q->income);
        numlarge=Trans_StrToNum(large->income);
        if(numq<numlarge){
            strcpy(t->name,q->name);
            strcpy(t->income,q->income);
            strcpy(t->date,q->date);
            strcpy(q->name,large->name);
            strcpy(q->income,large->income);
            strcpy(q->date,large->date);
            strcpy(large->name,t->name);
            strcpy(large->income,t->income);
            strcpy(large->date,t->date);
        }
        q=q->next;
        p=p->next;
    }
    free(t);
    p=first->next;
    printf("*******************************************\n\n");
    while(p!=NULL){
        if(!strcmp(p->name,user.name)){          //只输出登陆时名字的信息
            printf("          %s    %-6s %-6s\n",p->name,p->income,p->date);
        }
        p=p->next;
    }
    printf("\n*******************************************\n");
    printf("Enter '#' To Return:");
    scanf("%c",&x);
    if(getchar()=='#'){
        return num=Check;
    }
    return num=Date;
}

void Check_Income_Page(){
    int x;
    printf("*******************************************\n\n\n\n");
    printf("        1.Check by income\n\n");
    printf("        2.Check by date\n\n");
    printf("        3.Back\n");
    printf("\n\n\n*******************************************\n");
    printf("Please Enter Your Choice:");
    scanf("%d",&x);
    switch(x){
        case 1: num=Income;system("clear");break;
        case 2: num=Date;system("clear");break;
        case 3: num=Start;system("clear");break;
        default: exit(0);break;
    }
}

int Separate_Date(char*p,int choice){//choice为0分离年，choice为1分离月，choice 2分离日
    int r=0,i=0;
    char *t,*p1,*p2;
    char s[5];
    memset(s, 0, 5);//初始化s[5]
    switch (choice) {
        case 0:
            p1=p;
            p2=strchr(p,'-');
            for(t=p1;t<p2;t++,i++){
                s[i]=*t;
            }
            s[i+1]='\0';//逐个初始化字符串需要在最后加‘\0’
            r=Trans_StrToNum(s);
            break;
        case 1:
            p1=strchr(p,'-');
            p2=strrchr(p,'-');
            for(t=p1+1;t<p2;t++,i++){
                s[i]=*t;
            }
            s[i+1]='\0';//逐个初始化字符串需要在最后加‘\0’
            r=Trans_StrToNum(s);
            break;
        case 2:
            p1=strrchr(p, '-');
            p2=strrchr(p,'\0');
            for(t=p1+1;t<p2;t++,i++){
                s[i]=*t;
            }
            s[i+1]='\0';//逐个初始化字符串需要在最后加‘\0’
            r=Trans_StrToNum(s);
            break;
        default:
            break;
    }
    return r;
}

int YearlyIncome_Page(){
    Information *first,*p;
    int y,sum=0,yearp=0,income0;
    char x;
    printf("*******************************************\n\n\n\n\n\n");
    printf("        Please Enter The Year:");
    printf("\n\n\n\n\n\n*******************************************\n");
    gotoxy(31, 7);
    scanf("%d",&y);
    sleep(2);
    system("clear");
    printf("*******************************************\n\n");
    first=Create_List();
    first=Sort_Descending(first);
    p=first->next;
    while(p!=NULL){
        yearp=Separate_Date(p->date, 0);//0分离年份，1分离月份
        if((!strcmp(p->name, user.name))&&(yearp==y)){
            printf("          %s    %-6s %-6s\n",p->name,p->income,p->date);
            income0=Trans_StrToNum(p->income);
            sum=sum+income0;
        }
        p=p->next;
    }
    printf("\n*******************************************\n\n");
    printf("   The Total Income Of Year%d is %d\n\n",y,sum);
    printf("       Continue This Process?(Y/N):");
    scanf("%c",&x);
    if(getchar()=='N'){
        return num=Calculate;
    }
    return num=YearlyIncome;
}


int MonthlyIncome_Page(){
    Information *first,*p;
    int m,sum=0,monthp=0,income0,y,yp;
    char x;
    printf("*******************************************\n\n\n\n\n");
    printf("       Please Enter The Year-Month:\n");
    printf("\n\n\n\n\n\n*******************************************\n");
    gotoxy(17, 7);
    scanf("%d-%d",&y,&m);
    sleep(2);
    system("clear");
    printf("*******************************************\n\n");
    first=Create_List();
    first=Sort_Descending(first);
    p=first->next;
    while(p!=NULL){
        monthp=Separate_Date(p->date, 1);//0分离年份，1分离月份
        yp=Separate_Date(p->date, 0);
        if((!strcmp(p->name, user.name))&&(monthp==m)&&(yp==y)){
            printf("          %s    %-6s %-6s\n",p->name,p->income,p->date);
            income0=Trans_StrToNum(p->income);
            sum=sum+income0;
        }
        p=p->next;
    }
    printf("\n*******************************************\n\n");
    printf("   The Total Income Of %d-%d is %d\n\n",y,m,sum);
    printf("       Continue This Process?(Y/N):");
    scanf("%c",&x);
    if(getchar()=='N'){
        return num=Calculate;
    }
    return num=MonthlyIncome;
}

void Calculate_Income_Page(){
    int x;
    printf("*******************************************\n\n\n\n");
    printf("        1.Calculate Yearly Income\n\n");
    printf("        2.Calculate Monthly Income\n\n");
    printf("        3.Back\n");
    printf("\n\n\n*******************************************\n");
    printf("        Please Enter Your Choice:");
    scanf("%d",&x);
    switch(x){
        case 1: num=YearlyIncome;break;
        case 2: num=MonthlyIncome;break;
        case 3: num=Start;break;
        default: exit(0);break;
    }
}

void Initial_Page(){
    int x;
    printf("*******************************************\n\n\n");
    printf("                WELCOM:)\n\n");
    printf("               1.Log in\n\n");
    printf("               2.Sign up\n\n");
    printf("               3.Change Password\n");
    printf("\n\n*******************************************\n");
    printf("Please Enter Your Choice:");
    scanf("%d",&x);
    switch(x){
        case 1: num=Login;break;
        case 2: num=SignUp;break;
        case 3: num=Password;break;
        default: exit(0);break;
    }
}

int ChangePassword_Page(){
    Information new,t; int i;
    printf("*******************************************\n\n\n\n\n");
    printf("           Username:");
    printf("\n\n           New Password:");
    printf("\n\n\n\n\n*******************************************\n");
    gotoxy(21, 6);
    scanf("%s",user.name);
    gotoxy(25,8);
    scanf("%s",new.code);
    FILE *fp;
    fp=fopen("code.txt", "r+");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    while (fread(&t, sizeof(Information), 1, fp)!=0){
        if(!strcmp(t.name,user.name)){
            strcpy(user.code, new.code);
            fseek(fp,-sizeof(Information),SEEK_CUR);
            fwrite(&user, sizeof(Information), 1, fp );
            break;
        }
    }
    i=0;
    do{
        gotoxy(8, 10);
        printf("Successfully Change Password!\n");
        i++;
    }while(i<TIME);
    fclose(fp);
    return num=Initial;
}

int Login_Page(){
    Information t;int i; char r[1];
    printf("*******************************************\n\n\n\n\n");
    printf("           Username:");
    printf("\n\n           Password:");
    printf("\n\n\n\n\n*******************************************\n");
    gotoxy(21, 6);
    // fgets(user.name,20,stdin);//吞掉回车！单独在测试程序里有用在这里没用
    scanf("%s",user.name);
    r[0]=getchar();//吞掉回车
    gotoxy(21,8);
    getpasswd(user.code, 40);
    FILE *fp;
    fp=fopen("code.txt", "r");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    //检测用户名和密码是否被注册过
    while (fread(&t, sizeof(Information), 1, fp)!=0){
        if(!strcmp(t.name,user.name)){
            if(!strcmp(t.code,user.code)){
                break;
            }
            i=0;
            do{
                gotoxy(8, 10);
                printf("Password Is Incorrect!\n");
                i++;
            }while(i<TIME);
            fclose(fp);
            return num=Login;
        }
    }
    fseek(fp,-sizeof(Information),SEEK_CUR); //使文件指针指向上一个，
    if(!strcmp(t.name,user.name)&&!strcmp(t.code,user.code)){
        fclose(fp);
        return num=Start;
    }
    i=0;
    do{
        gotoxy(10, 10);
        printf("Username Doesn't Exist!\n");
        i++;
    }while(i<TIME);
    fclose(fp);
    return num=Login;
}

int SignUp_Page(){
    char code2[40],code1[40]; int i=0; char r[1];
    Information newuser,t;
    printf("*******************************************\n\n\n\n");
    printf("        Enter a Username:");
    printf("\n\n        Enter Your Password:");
    printf("\n\n        Reenter Your Password:");
    printf("\n\n\n\n\n\n*******************************************\n");
    gotoxy(26, 5);
    scanf("%s",newuser.name);
    r[0]=getchar();//吞掉回车！！！
    //将新建用户名存入文件中
    FILE *fp;
    fp=fopen("code.txt", "r+t");
    if(fp==NULL){
        printf("Can't open this file\n");
        exit(0);
    }
    //先扫描文件检验新的username是否已经存在
    while (fread(&t, sizeof(Information), 1, fp)!=0) {
        if(strcmp(t.name, newuser.name)==0){
            i=0;
            do{
                gotoxy(6,11);
                printf("This name is already used!\n");
                i++;
            }while(i<TIME);
            fclose(fp);
            return num=SignUp;
        }
    }
    gotoxy(29, 7);
    getpasswd(code1, 40);
    gotoxy(31, 9);
    getpasswd(code2, 40);
    //两次输入密码不相同需要重新输入
    if((strcmp(code1, code2))!=0){
        i=0;
        do{
            gotoxy(6, 11);
            printf("Inconsistent Password!Try Again!\n");
            i++;
        }while(i<TIME);
        fclose(fp);
        return num=SignUp;
    }
    strcpy(newuser.code, code1);
    //写入新的用户名及其密码信息
    fwrite(&newuser, sizeof(Information), 1, fp);
    fclose(fp);
    i=0;
    do{
        gotoxy(6, 11);
        printf("Create a new user successfully!\n");
        i++;
    }while(i<TIME);
    return num=Initial;
}
