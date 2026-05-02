#include <iostream>
#include <string>
using namespace std;

struct course
{
    int id;
    int start;
    int end;
    course* next;
    course(int i, int s, int e) : id(i), start(s), end(e), next(nullptr) {}
};
course* head = nullptr;

bool course_exist(int id) {
    course * curr = head;
    while (curr != nullptr) {
        if (curr->id == id) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

bool course_confict(int start, int end, int self = -1) {
    course * curr = head;
    while (curr != nullptr) {
        if (curr ->id == self) {
            curr = curr -> next;
            continue;
        }
        if (curr->start < end && curr->end > start){
            return true;
        }
        curr = curr->next;
        }
    return false;
        }
    
void order(int id, int start, int end) {
    course * new_course = new course(id, start, end);
    if(head == nullptr || new_course->start < head -> start) {
         new_course -> next = head;
         head = new_course;
         return;
    }
    course * curr = head;
    while (curr->next != nullptr && curr->next->start < new_course->start) {
        curr = curr->next;
    }
    new_course -> next = curr -> next;
    curr -> next = new_course;
}

void insert_course(int id, int start, int end) {
    if (start >= end) { 
        cout << "false" << endl;
        return;
    }
    if (course_exist(id)) {
        cout << "false" << endl;
        return;
    }
    if (course_confict(start,end)) {
        cout << "false" << endl;
        return;
    }
    order(id,start,end);
}

void delete_course(int id) {
    if (head == nullptr) {
        return;
    }
    course * curr = head;
    course * prev = nullptr;
    if (curr-> id == id) {
        head = head-> next;
        delete curr;
        return;
    }

    while (curr != nullptr && curr->id != id) {
        prev = curr;
        curr = curr->next;
    }
    if (curr != nullptr) {
        prev->next = curr->next;
        delete curr;
    }
}

void update(int id, int new_start, int new_end) {
    if (new_start >= new_end){
        cout << "false" << endl;
        return;
    }
    if (!course_exist(id)) {
        cout << "false" << endl;
        return;
    }
    if (course_confict(new_start, new_end, id)) {
        cout << "false" << endl;
        return;
    }

    delete_course(id);
    order(id, new_start, new_end);
}

void print(){
    if (head == nullptr) {
        cout << "false" << endl;
        return;
    }
    course * curr = head;
    while (curr != nullptr) {
        cout << curr->id << " " << curr->start << " " << curr->end << endl;
        curr = curr->next;
    }
}



int main(){
    string operation;
    while (operation != "quit"){
        cin >> operation;
        if (operation == "insert") {
            int id, start, end;
            cin >> id >> start >> end;
            insert_course(id,start,end);

        }else if (operation == "delete") {
            int id;
            cin >> id;
            delete_course(id);
        } else if (operation == "update"){
            int id,new_start,new_end;
            cin >> id >> new_start >> new_end;
            update(id,new_start,new_end);

        } else if (operation == "print"){
            print();
        } 
    }
    
    return 0;
}
    