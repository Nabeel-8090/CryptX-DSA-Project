#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
 
using namespace std;

fstream file;
int menu();
bool VerifyPass();
void UserLogin();
void RegisterUser();
 
int main(){
    
    menu();
 
 
  return 0;
}

int menu(){
    int option;

    cout<<".........Please Select An Option......... \n";
    cout<<"......... 1.User login.........\n";
    cout<<"......... 2.Register User.........\n";
    cin>> option;

    switch(option){
        case 1: UserLogin();
                return 0;
        case 2: RegisterUser();
            return 0;
    }
    return 0;
}

void UserLogin(){
    int option;
    if(VerifyPass()){
        cout<<".........Welcome User! You are logged in successfully......... \n";
    }
}

bool VerifyPass(){
    string username, password;
    string fileUsername, filePassword;
    bool validUsername = false;
    bool validPassword = false;

    cout<<"Please Enter Your Username: ";
    cin.ignore();
    getline(cin, username);
    
    cout<<"Please Enter Your Password: ";
    getline(cin, password);

    file.open("users.txt", ios:: in);

    if(!file.is_open()){
        cout<<"error opening the file";
        return false;
    }

    int line = 1;
    while(getline(file, line % 2 != 0 ? fileUsername : filePassword)){
        if(line % 2 != 0 && fileUsername == username){
            validUsername = true;
        }
        else if(line % 2 == 0 && filePassword == password){
            validPassword = true;
        }
        line++;
    }
    file.close();

    cout<<validUsername<<" "<<validPassword<<endl;
    if(validUsername && validPassword){
        return true;
    }
    else{
        return false;
    }
}

void RegisterUser(){
    string username, password;
    file.open("users.txt", ios:: app);

    if(!file.is_open()){
        cout<<"error opening the file";
    }

    cout<<"Please Enter Your Username: ";
    cin.ignore();
    getline(cin, username);
    file<<username<<endl;
    
    cout<<"Please Enter Your Password: ";
    getline(cin, password);
    file<<password<<endl;
    

    file.close();
    
    cout<<"You've registered successfully Mr."<<username;
    menu();
    return;

}
