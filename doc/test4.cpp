// ---------------------------------------------------------
#include <iostream.h>
#include <string.h>
#include <mgr.h>  // always include
#include <aggregat.h>
class Department;
class Employee;
class Project;
class Department : public AggregateParent<Department,Department>,
                   public AggregateChild<Department,Department>,
                   public AggregateParent<Department,Employee>,
                   public AggregateParent<Department,Project>{
    int mDeptNo;
    void prtOffset(int i);
public:
    Department(int n){mDeptNo=n;}
    void prt(int offset); // print this and subordinate departments
    Employee* find(char *name); // recursively, find this employee
};
class Employee : public AggregateChild<Department,Employee>{ 
    char *mpName;
public:
    Employee(char *n){mpName=new char[strlen(n)+1]; strcpy(mpName,n);}
    void prt(void){cout << " " << mpName;}
    int checkName(char *n){return !strcmp(mpName,n);}
};
class Project : public AggregateChild<Department,Project>{ 
    char mProj;
public:
    Project(char p){mProj=p;}
    void prt(void){cout << " " << mProj;}
};
// ---------------------------------------------------
// The following lines declare the data organization.
// These objects do not have to be global, but making them
// global often contributes to the clarity of the code
// ---------------------------------------------------
Aggregate<Department,Department> departments;
Aggregate<Department,Employee> employees;
Aggregate<Department,Project> projects;
// ---------------------------------------------------
void Department::prtOffset(int i){
    cout << "\n";
    for(int k=0; k<i; k++) cout << "    ";
}
void Department::prt(int offset){
    Employee *pEmpl;
    Project *pProj;
    Department *pDept;
    AggregateIterator<Department,Department> deptIt;
    AggregateIterator<Department,Employee>   emplIt;
    AggregateIterator<Department,Project>    projIt;
    prtOffset(offset);
    cout << "department= " << mDeptNo;
    if(employees.head(this)){
        prtOffset(offset);
        cout << "employees:";
        ITERATE(emplIt,this,pEmpl) pEmpl->prt();
    }
    if(projects.head(this)){
        prtOffset(offset);
        cout << "projects:";
        ITERATE(projIt,this,pProj) pProj->prt();
    }
    cout << "\n";
    // recursively, print all subordinated departments
    ITERATE(deptIt,this,pDept) pDept->prt(offset+1);
}
    
Employee* Department::find(char *name){
    Employee *pEmpl;
    Department *pDept;
    AggregateIterator<Department,Department> deptIt;
    AggregateIterator<Department,Employee>   emplIt;
    // traverse the employees of this department
    ITERATE(emplIt,this,pEmpl){
        if(pEmpl->checkName(name))return pEmpl;
    }
    // recursively, search subordinate departments
    ITERATE(deptIt,this,pDept){
        pEmpl=pDept->find(name);
        if(pEmpl)return pEmpl;
    }
    return NULL;
}
    
int main(void){
    Department *pDept0,*pDept1,*pDept2;
    Employee *pEmpl;
    Project *pProj;
    
    // create a small hierarchy of departments under pDept0
    // To each department, add some employees and projects
    pDept0=new Department(200);
        pEmpl=new Employee("Head F."); employees.addTail(pDept0,pEmpl);
    pDept1=new Department(220); departments.addTail(pDept0,pDept1);
        pEmpl=new Employee("Mann H."); employees.addTail(pDept1,pEmpl);
    pDept2=new Department(225); departments.addTail(pDept1,pDept2);
        pEmpl=new Employee("Brown J."); employees.addTail(pDept2,pEmpl);
        pEmpl=new Employee("Green D."); employees.addTail(pDept2,pEmpl);
        pEmpl=new Employee("Black S."); employees.addTail(pDept2,pEmpl);
        pProj=new Project('A'); projects.addTail(pDept2,pProj);
    pDept2=new Department(228); departments.addTail(pDept1,pDept2);
        pEmpl=new Employee("Little K."); employees.addTail(pDept2,pEmpl);
        pEmpl=new Employee("Grossman A."); employees.addTail(pDept2,pEmpl);
        pProj=new Project('B'); projects.addTail(pDept2,pProj);
        pProj=new Project('C'); projects.addTail(pDept2,pProj);
    pDept1=new Department(240); departments.addTail(pDept0,pDept1);
        pEmpl=new Employee("Boss I."); employees.addTail(pDept1,pEmpl);
    pDept2=new Department(243); departments.addTail(pDept1,pDept2);
        pEmpl=new Employee("Zeller K."); employees.addTail(pDept2,pEmpl);
        pEmpl=new Employee("Chapeau A."); employees.addTail(pDept2,pEmpl);
        pEmpl=new Employee("Krpec P."); employees.addTail(pDept2,pEmpl);
        pProj=new Project('D'); projects.addTail(pDept2,pProj);
        pProj=new Project('E'); projects.addTail(pDept2,pProj);
        pProj=new Project('F'); projects.addTail(pDept2,pProj);
    // Boss I. leaves the company
    pEmpl=pDept0->find("Boss I."); 
    employees.remove(pEmpl);
    delete pEmpl;
    // Chapeau A. is promoted as head of department 240
    pEmpl=pDept2->find("Chapeau A."); 
    employees.remove(pEmpl); 
    employees.addTail(pDept1,pEmpl);
    pDept0->prt(0); // print the entire organization
    return(0);
}
/* ---------------------- results -----------------------
department= 200
employees: Head F.
    department= 220
    employees: Mann H.
        department= 225
        employees: Brown J. Green D. Black S.
        projects: A
        department= 228
        employees: Little K. Grossman A.
        projects: B C
    department= 240
    employees: Chapeau A.
        department= 243
        employees: Zeller K. Krpec P.
        projects: D E F
   ----------------------------------------------------*/
