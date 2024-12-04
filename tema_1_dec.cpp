#include <iostream>
#include <string>
#include <cctype>
#include <regex>
#include <fstream>
#include <set>
#include <utility>
#include <sstream>
#include <tuple>
#include <ctime>
#include <map>
#include "sha256.h"
using namespace std;


// ----------------------------------------------------------------------------
//                   Algoritm pt verificarea parolei
// ----------------------------------------------------------------------------
void pass_strength_factors(string password){

    if(password.length()<12){
        throw((string)"Parola este prea scurtă! (min 12 caractere)");
    }    
    string test = password.c_str();

    // Convert every character of string to
    // uppercase using toupper() method
    for (int i = 0; i < test.length(); i++)
        test[i] = toupper(test[i]);

    if(test==password){
        // trebuie să fac aici type conversion; altfel, îl percepe ca fiind doar const char
        throw((string)"Parola nu conține caractere lowercase");
    }
    
    test = password;
    for (int i = 0; i < test.length(); i++)
        test[i] = tolower(test[i]);

    if(test==password){
        // trebuie să fac aici type conversion; altfel, îl percepe ca fiind doar const char
        throw((string)"Parola nu conține caractere uppercase");
    }

    //pt caractere speciale și numere o să fac regex (puteam pt toate să fac regex, dar *cred* că e cv mai rapid așa pt lowercase și suppercase)
    regex num_regex("[0-9]+");
    if(!regex_search(password, num_regex)){
        throw(-1);
    }
    
    // a trebuit să scot câteva caractere speciale ptc nu se potriveau cu parse-uriea de stringuri nativă c++
    regex special_regex("[!@#$%^&*()_=+{};:<>,./?]+");
    if(!regex_search(password, special_regex)){
        throw(1);
    }
    
}

/*
    output-ul funcției

    -1 - parolă slabă - dacă parola are mai puțin de 12 caractere și/sau nu are caractere mici și/sau nu are caractere mari
     0 - parolă medie - dacă parola nu e slabă, dar nu are c.p. o cifră sau un caracter special
     1 - parolă bună - îndeplinește toate condițiile menționate anterior

    am stabilit ca verificările pt cifră și caracter să returneze excepții prin int pt a le putea grupa diferit
*/
int password_strength(string password){
    try{
        pass_strength_factors(password);
    }
    catch(string error){
        cout<<error<<endl;
        return -1;
    }
    catch(int error_code){
        if(error_code==-1){
            cout<<"Parola nu conține nicio cifră!"<<endl;
        }else{
            cout<<"Parola nu conține niciun caracter special!"<<endl;
        }

        return 0;
    }catch(...){
        cout<<"Eroare neprevăzută. Încercați să vă înregistrați încă odată."<<endl;
        return -1;
    }
    return 1;
}

// clasa pt a parsa inputul de text
// preia numele fișierului și stochează liniile .csv-ului într-un vector de string

class CSV_input{
    private:

    string in_filename;

    public:

    vector<string> elements;

    CSV_input(string filename){
        ifstream fin(filename);
        //cout<<filename<<endl;
        //fin.open(filename, ios::in);
        
        in_filename = filename;
        string line;

        if(fin.is_open()){
           // cout<<"open"<<endl;
            while(getline(fin, line)){
                elements.push_back(line);
            }
        }

        // Afișare pt debugging
        /*
        cout<<elements.size()<<endl;
        for(int i=0; i<elements.size(); i++){
            cout<<elements[i]<<endl;
        }
        */


        fin.close();
    }

    // dacă vrem să schimbăm cv în CSV în mod direct, putem face direct din funcția asta
    void write_to_file(){
        //cout<<"filename test "<<in_filename<<endl;
        ofstream fout(in_filename);

        for(int i=0; i<elements.size(); i++){
            fout<<elements[i]<<endl;
        }

        fout.close();
    }

    void write_line(string new_line){
        ofstream output;
        output.open(in_filename, ios_base::app);

        output<<new_line;
    }
};


// clasa pt cursă
class TrainItinerary{
    private:
        int id;
        string source;
        string destination;
        vector<string> stations;
        // pt timp(uri), voi trece doar timpul de plecare din stație și cel de ajungere în stația finală
        // nu mai sciu și pt cele intermediare
        string departure_time;
        string departure_day;
        string arrival_time;
        string arrival_day;
        // aici o să am un set (poate schimb în array) de perechi vagon-loc pt a stoca locurile ocupate
        vector<pair<int,int>> seats_taken;
    
    public:
        // am nevoie să accesez direct stations în clasa User
        friend class User;

        // id global folosit pt generarea elementelor nou-adăugate
        static int global_id;

        TrainItinerary(int id_in, string source_in, string destination_in, vector<string> stations_in, string departure_time_in, string departure_day_in, string arrival_time_in, string arrival_day_in, vector<pair<int,int>> seats_taken_in){
            id = id_in;
            source = source_in;
            destination = destination_in;
            stations = stations_in;
            departure_time = departure_time_in;
            departure_day = departure_day_in;
            arrival_time = arrival_time_in;
            arrival_day = arrival_day_in;
            seats_taken = seats_taken_in;
        }
        
        //scrie elementele într-un singur string, menit să fie linie din CSV
        string write_to_line(){
            string output = "";
            output.append(to_string(id));
            output.append(",");

            output.append(source);
            output.append(",");

            output.append(destination);
            output.append(",");

            for(int i=0; i<stations.size(); i++){
                output.append(stations[i]);
                if(i!=stations.size()-1){
                    output.append("-");
                }
            }
            output.append(",");

            output.append(departure_time);
            output.append(",");

            output.append(departure_day);
            output.append(",");

            output.append(arrival_time);
            output.append(",");

            output.append(arrival_day);
            output.append(",");

            for(int i=0; i<seats_taken.size();i++){
                output.append(to_string(seats_taken[i].first));
                output.append(".");
                output.append(to_string(seats_taken[i].second));
                if(i!=seats_taken.size()-1){
                    output.append("-");
                }
            }
            return output;
        }

        int get_id(){
            return id;
        }

        string get_departure_day(){
            return departure_day;
        }
};

int TrainItinerary::global_id = 1;

// checks that a string can be used as a station name (only a-z, A-Z and spaces)
void check_string(string input){
    regex text_regex(R"([a-zA-Z ]+)");
    if(!regex_match(input, text_regex)){
        throw(-1);
    }
}
// funcție fără argumente, cu introducerea datelor de la tastatură
// (puteam să o fac constructor)
TrainItinerary itinerary_from_keyboard(){
    int id_temp;
    string source_temp;
    string destination_temp;
    int number_stations;
    vector<string> stations_temp;
    string departure_time_temp;
    string departure_day_temp;
    string arrival_time_temp;
    string arrival_day_temp;
    int seats_taken_number;
    vector<pair<int,int>> seats_taken_temp;

    cout<<"Introduceți stația din care pleacă cursa: "<<endl;
    cin>>source_temp;
    check_string(source_temp); //puțin dubios, cum ar cam trb să bag restul funcției în try-catch
    cout<<"Introduceți stația la care ajunge"<<endl;
    cin>>destination_temp;
    check_string(destination_temp);
    stations_temp.push_back(source_temp);
    
    // TODO: funcție pt a verifica dacă textul este valid (dacă e doar ASCII, nu caractere UTF)
    cout<<"Introduceți câte stații sunt între "<<source_temp<<" și "<<destination_temp<<endl;
    cin>>number_stations;
    string intermediary_station_temp;
    for(int i=1; i<=number_stations; i++){
        cout<<"Introduceți stația #"<<i<<endl;
        try{
            cin>>intermediary_station_temp;
            stations_temp.push_back(intermediary_station_temp);
            check_string(intermediary_station_temp);
        }catch(int err){
            cout<<"Numele stației #"<<i<<" conține caractere invalide (probabil diacritice)"<<endl;
            throw(err);
        }
        
    }
    stations_temp.push_back(destination_temp);

    // TODO: verificarea orei/zilei de plecare (să fie după ora/ziua curentă)
    cout<<"Introduceți ora de plecare: "<<endl;
    cin>>departure_time_temp;
    cout<<"Introduceți ziua plecării: "<<endl;
    cin>>departure_day_temp;
    cout<<"Introduceți ora de sosire: "<<endl;
    cin>>arrival_time_temp;
    cout<<"Introduceți ziua sosirii: "<<endl;
    cin>>arrival_day_temp;

    // aici am să sper că îmi merge programul dacă vectorul seats_taken_temp e gol
    // dacă nu, atunci o să pun o pereche (-1,-1) fictivă
    int num_seats;
    cout<<"Introduceți numărul de locuri disponibile: "<<endl;
    cin>>num_seats;
    int wagon_temp, seat_temp;
    for(int i=1; i<=num_seats; i++){
        cout<<"Introduceți perechea #"<<i<<" (vagon, loc): "<<endl;
        cin>>wagon_temp>>seat_temp;
        seats_taken_temp.push_back(make_pair(wagon_temp, seat_temp));
    }  

    id_temp = TrainItinerary::global_id;
    TrainItinerary::global_id++;

    cout<<"test diacritice: "<<source_temp<<" "<<destination_temp<<endl;
    TrainItinerary output(id_temp,source_temp,destination_temp,stations_temp,departure_time_temp,departure_day_temp,arrival_time_temp,arrival_day_temp,seats_taken_temp);
    return output;
}


TrainItinerary itinerary_from_string(string input){
    int id_temp = TrainItinerary::global_id;
    string source_temp;
    string destination_temp;
    vector<string> stations_temp;
    string departure_time_temp;
    string departure_day_temp;
    string arrival_time_temp;
    string arrival_day_temp;
    vector<pair<int,int>> seats_taken_temp;

    stringstream s(input);
    string element_attribute;
    string temp_attribute; //pt cei 2 vectori
    getline(s, element_attribute, ',');
    id_temp = stoi(element_attribute);
    getline(s, element_attribute, ',');
    source_temp = element_attribute;
    getline(s, element_attribute, ',');
    destination_temp = element_attribute;
    getline(s, element_attribute, ',');
    
    stringstream s_temp(element_attribute);
    while(getline(s_temp, temp_attribute, '-')){
        //cout<<temp_attribute<<endl;
        stations_temp.push_back(temp_attribute);
    }

    getline(s, element_attribute, ',');
    departure_time_temp = element_attribute;
    getline(s, element_attribute, ',');
    departure_day_temp = element_attribute;
    getline(s, element_attribute, ',');
    arrival_time_temp = element_attribute;
    getline(s, element_attribute, ',');
    arrival_day_temp = element_attribute;

    getline(s, element_attribute, ',');
    
    s_temp = stringstream(element_attribute);
    while(getline(s_temp, temp_attribute, '-')){
        //cout<<temp_attribute<<endl;

        string first_num = temp_attribute.substr(0, temp_attribute.find('.'));
        string second_num = temp_attribute.substr(temp_attribute.find('.')+1);
        pair<int,int> temp_pair = make_pair(stoi(first_num), stoi(second_num));
        //cout<<temp_pair.first<<" "<<temp_pair.second<<endl;
        seats_taken_temp.push_back(temp_pair);
    }
    
    TrainItinerary output(id_temp, source_temp, destination_temp, stations_temp, departure_time_temp, departure_day_temp, arrival_time_temp, arrival_day_temp, seats_taken_temp);
    return output;

}

// funcție care extrage din fișierul cu lista de curse vector de obiecte TrainItinerary
vector<TrainItinerary> get_itinerary_list(string filename){
    vector<TrainItinerary> itinerary_arr;
    CSV_input input(filename);

    for (int i = 0; i < input.elements.size(); i++){
        itinerary_arr.push_back(itinerary_from_string(input.elements[i]));
    }

    // aici o să mă uit la ultimul element din vector și îi stochez ultimul ID incrementat
    // o problemă ar fi că așa id-ul poate doar să crească, cum nu e decrementat dacă ștergi un element din interiorul vectorului
    // dar nu cred că e cine-știe-ce problemă
    if(itinerary_arr.size()>0){
        TrainItinerary::global_id = itinerary_arr.back().get_id()+1;
    }
    
    return itinerary_arr;
}

// string pt data
vector<tuple<int,int,int,string>> get_ticket_list(string filename, string username){
    vector<tuple<int,int,int,string>> output;
    tuple<int,int,int,string> tuple_temp;
    CSV_input input(filename);
    int user_found = 0;

    string element, tuple_number, user_tickets_temp;

    // căutăm mai întâi utilizatorul în CSV
    for (int i = 0; i< input.elements.size(); i++){
        stringstream user_string(input.elements[i]);
        getline(user_string, element, ',');

        //cout<<"test get_ticket_list: "<<element<<endl;
        if(username==element){
            user_found = 1;
            //cout<<"matched"<<endl;
            getline(user_string, element, ',');
            user_tickets_temp = input.elements[i];

            // acum avem lista de tupluri, pe care o putem adăuga
            while(getline(user_string, element, ';')){
                stringstream temp(element);
                
                getline(temp, tuple_number, '-');
                get<0>(tuple_temp) = stoi(tuple_number);

                getline(temp, tuple_number, '-');
                get<1>(tuple_temp) = stoi(tuple_number);

                getline(temp, tuple_number, '-');
                get<2>(tuple_temp) = stoi(tuple_number);
                
                getline(temp, tuple_number, '-');
                get<3>(tuple_temp) = stoi(tuple_number);
            }

            // programul va elimina stringul curent cu username-ul curent ca să-l adauge iarăși la final, în destructorul User-ului
            input.elements.erase(input.elements.begin()+i);

            //cout<<"test stergere"<<endl;
            //for(int i=0; i<input.elements.size(); i++){
            //    cout<<input.elements[i]<<endl;
            //}
            input.write_to_file();
            input.write_line(user_tickets_temp);

            break;
        }
    }
    
    
    if(!user_found){
        cout<<"Utilizator nou; adăugat în CSV"<<endl;
        ofstream ticket_output;
        ticket_output.open(filename, ios_base::app);
        ticket_output<<string(username+",");
    }
    

    return output;
}

// ----------------------------------------------------------------------------
//                   Clasa de Login
// ----------------------------------------------------------------------------
class Login{
    // cum atât Operatorul, cât și utilizatorul, se loghează în același fel, folosesc amândoi aceeași clasă de Login
    // (iarăși, ar fi fost bine să fi făcut o clasă de User care să fie moștenită de ambele, dar n-am avut timp să fac refactoring)

    private:
    
    // numele fișierului din care sunt extrase datele
    string filename;
    map<string,string> login_map; // map care o să stocheze perechi email-parolă
    // cheia de la codul vigenere o să fie unică, cum oricum o să hashuiesc parolele

    void obtain_list(string filename){
        CSV_input input(filename);

        for(int i=0; i<input.elements.size(); i++){
           
            int pos = input.elements[i].find_first_of(',');
            login_map.insert(pair<string,string>(input.elements[i].substr(0, pos),input.elements[i].substr(pos+1))); // adăugăm perechea username-parolă în hashmap       
        }

        //for(const auto& elem : login_map){
        //    std::cout << elem.first << " " << elem.second << "\n";
        //}
    }

    void append_list(string username, string password){
        string to_output = username+','+password;
        CSV_input input(filename);

        input.write_line(to_output);
    }
    
    public:

    Login(string filename_in){
        filename = filename_in;

        obtain_list(filename);
    }

    string register_user(){
        string username_temp, password_temp, password_check_temp;  
        // important: adresa nu poate avea , din cauza CSV-ului
        const string regex_string = R"([a-zA-Z0-9_.]+@[a-zA-z0-9]+.[a-zA-Z]+)";
        const regex email_regex(regex_string);
        
        cout<<"Introduceți adresa de email: "<<endl;
        cin>>username_temp;
        if(login_map.find(username_temp) != login_map.end()){ //cond îndeplinită dacă adresa este în map
            cout<<"Deja este înregistrat un cont cu această adresă!"<<endl;
            throw(2);
        }
        // acum verific dacă stringul dat este o adresă de email validă cu un regex
        if(!regex_match(username_temp, email_regex)){
            //cout<<username_temp<<'|'<<regex_match(username_temp, email_regex)<<endl;
            cout<<"Adresa furnizată nu este validă!"<<endl;
            throw(3);
        }

        cout<<"Introduceți noua parolă:"<<endl;
        cin>>password_temp;
        if(password_strength(password_temp)!=1){
            //cout<<"Înregistrarea nu a putut fi făcută!"<<endl;
            throw(4);
        }
        
        cout<<"Introduceți iarăși noua parolă"<<endl;
        cin>>password_check_temp;
        if(password_temp!=password_check_temp){
            cout<<"Parolele date nu sunt identice!"<<endl;
            throw(5);
        }
        
        SHA256 sha256;
        //TODO: prostia aia cu vigenere
        string pass_stored = sha256(password_temp);
        CSV_input(filename).write_line(string("\n")+username_temp+string(",")+pass_stored); //fac string() ca să pot folosi operatorul overloaded +

        return username_temp;
    }

    string login_user(){
        string username_temp, password_temp;

        cout<<"Introduceți adresa de email:"<<endl;
        cin>>username_temp;
        if(login_map.find(username_temp) == login_map.end()){ //cond îndeplinită utilizatorul cu ac adresă nu este înregistrat
            cout<<"Nu există niciun cont cu această adresă de email! "<<endl;
            throw(1);
        }

        cout<<"Introduceți parola:"<<endl;
        cin>>password_temp;

        SHA256 sha256;
        string password_hash = sha256(password_temp);
        if (login_map[username_temp]!=password_hash){
            cout<<"Parola introdusă este greșită!"<<endl;
            throw(1);
        }

        return username_temp;
    }
};


// ----------------------------------------------------------------------------
//                   Clasa de Operator
// ----------------------------------------------------------------------------
// TODO: alg de autentificare
class Operator{
    private:

    string username;
    // parola hash-uită
    string password;

    // string care conține numele fișierului de input/output
    string itinerary_file;
    vector<TrainItinerary> itinerary_arr;
    string accounts_file;

    public:

    Operator(){
        // AICI INTRODUC PARTEA DE LOGIN
        // (o să pun în altă funcție partea de logare concretă, că pot să refolosesc din ea și la clasa de User)

        // pe asta o decomentez după ce am informații în fișier
        // ATENTIE: aici ar putea să meargă prost dacă VSCode nu deschide fișierul cu executabilul
        // dacă se deschide din alt fișier mai sus în ierarhie, nu citește bine
        itinerary_file = "train_itinerary.csv";
        accounts_file = "operator_list.csv";
        itinerary_arr = get_itinerary_list(itinerary_file);


        Login account_login(accounts_file);
        char user_selection='z';
        cout<<"1) Înregistrare\n"<<"2) Logare"<<endl;
        while(user_selection!='1'&&user_selection!='2'){
            cin>>user_selection;
        }

        if(user_selection=='1'){
            username = account_login.register_user();
        }else if(user_selection=='2'){
            username = account_login.login_user();
        }
    }

    // toate operațiilor asupra listelor de curse am să le fac în vectorul itinerary_arr
    // pentru a scrie în fișier schimbările, suprascriu fișierul de input cu elementele din vector
    // (pot face asta doar ptc asum că este doar o singură instanță la un moment dat)
    ~Operator(){
        ofstream output;
        output.open(itinerary_file);

        for(int i=0; i<itinerary_arr.size(); i++){
            output<<itinerary_arr[i].write_to_line()<<endl;
        }

        output.close();
    }
    
    // am să creez atât o funcție internă cu care pot adăuga eu TrainItineraries manual, cât și una care îmi permite să introduc din CLI
    
    void add_itinerary(TrainItinerary element){
        itinerary_arr.push_back(element);
    }

    void add_itinerary(){
        TrainItinerary element = itinerary_from_keyboard();
        itinerary_arr.push_back(element);
    }
    
    // Funcție care șterge cursa în funcție de ID
    void remove_itinerary(int id){
        int id_found = 0;
        // aici aș putea face căutare binară, cum ID-urile sunt adăugate în ordine crescătoare
        for(int i=0; i<itinerary_arr.size(); i++){
            if (itinerary_arr[i].get_id() == id){
                itinerary_arr.erase(itinerary_arr.begin()+i);
                cout<<"Cursa cu ID-ul "<<id<<" a fost ștearsă cu success!"<<endl;
                id_found = 1;
                break;
            }
        }
        if(id_found==0){
            cout<<"Cursa cu ID-ul "<<id<<" nu a putut fi găsită!"<<endl;
        }
    }
};


// ----------------------------------------------------------------------------
//                   Clasa de Utilizator
// ----------------------------------------------------------------------------
class User{
    // aici refolosesc ft mult cod de la clasa Operator
    // ar fi fost mai bine (și mai logic) să fi avut o clasă Actor, pe care să o moștenească atât User, cât și Operator
    // totuși, nu mai am suficient timp pt a face refactoring, deci cam las așa
    // o să fac cu moștenire în Kotlin tho

    private:

    string username;
    // parola hash-uită
    string password;

    // string care conține numele fișierului de input/output
    string itinerary_file; 
    string ticket_file; // fișier separat pt biletele înregistrate
    string accounts_file;
    
    vector<TrainItinerary> itinerary_arr;
    vector<tuple<int,int,int,string>> ticket_arr; // primul int este id-ul trenului, al doilea este vagonul (default -1, dacă nu a optat pt asta), ai treilea este locul (default -1, idem)

    public:

    User(){
        // AICI INTRODUC PARTEA DE LOGIN
        // (o să pun în altă funcție partea de logare concretă, că pot să refolosesc din ea și la clasa de User)

        // pe asta o decomentez după ce am informații în fișier
        // ATENTIE: aici ar putea să meargă prost dacă VSCode nu deschide fișierul cu executabilul
        // dacă se deschide din alt fișier mai sus în ierarhie, nu citește bine

        //username = "example@domain.com";

        itinerary_file = "train_itinerary.csv";
        ticket_file = "user_tickets.csv";
        accounts_file = "user_list.csv";

        itinerary_arr = get_itinerary_list(itinerary_file);
        //ticket_arr = get_ticket_list(ticket_file, username);


        Login account_login(accounts_file);
        char user_selection='z';
        cout<<"1) Înregistrare\n"<<"2) Logare"<<endl;
        while(user_selection!='1'&&user_selection!='2'){
            cin>>user_selection;
        }

        if(user_selection=='1'){
            username = account_login.register_user();
        }else if(user_selection=='2'){
            username = account_login.login_user();
        }
    }
    
    // dacă era să permit să se logheze mai mulți utilizatori/operatori în aceeași sesiune, aș face o funcție separată de write_changes()
    ~User(){
        ofstream output;
        output.open(itinerary_file);

        for(int i=0; i<itinerary_arr.size(); i++){
            output<<itinerary_arr[i].write_to_line()<<endl;
        }

        output.close();


    }


    void show_trains(){
        string source, destination, day;

        // TODO: de pus și aici excepții pt input
        cout<<"Introduceți stația de plecare (fără diacritice): "<<endl;
        cin>>source;
        cout<<"Introduceți stația destinație (fără diacritice): "<<endl;
        cin>>destination;
        cout<<"În ce zi vreți să plecați din stație? (format dd/mm/yyyy): "<<endl;
        cin>>day;
        // TODO: verificare pt curse din trecut

        // cout<<day<<endl;
        int trips_found = 0;
        // vectorul stations include stațiile în ordinea în care sunt parcurse, deci putem să căutăm secvențial
        // (algoritmul ăsta este ~O(n^3/2), dar nu prea se poate mai bine cu structurile de date folosite)
        // (dacă am menține cv graf al stațiilor, e posibil să scoatem o eficiență mai bună)
        for(int i=0; i<itinerary_arr.size(); i++){
            for(int j=0; j<itinerary_arr[i].stations.size(); j++){
                //verific mai întâi dacă stația de plecare se potrivește
                if(itinerary_arr[i].stations[j] == source){
                    for(int k=j+1; k< itinerary_arr[i].stations.size(); k++){
                        if(itinerary_arr[i].stations[k] == destination){
                            // cout<<itinerary_arr[i].get_departure_day()<<endl;
                            if(itinerary_arr[i].get_departure_day() == day){
                                trips_found++;
                                cout<<itinerary_arr[i].write_to_line()<<endl;
                            }
                        }

                    }
                }
            }
        }
        if(trips_found==0) cout<<"Nu există curse în intervalul de timp specificat!"<<endl;
    }
    
    void buy_ticket(int train_id){
        tuple<int,int,int,string> new_ticket;
        int ticket_class;
        char rasp;
        int wagon = -1; // default
        int seat = -1;
        
        // ignorăm aici faptul că sunt vagoane diferite pt clasele 1 și 2

        // TODO: exception handling pt asta
        cout<<"Ce clasă doriți, 1 sau 2?"<<endl;
        cin>>ticket_class;
        if(cin.fail()){
            throw(-1);
            return;
        }
        if((ticket_class!=1)&&(ticket_class!=2)){
            throw(1);
            return;
        }

        cout<<"Doriți să rezervați și loc? (d/n)";
        cin>>rasp;
        if(cin.fail()){
            throw(-1);
            return;
        }

        // dacă pun orice în afară de 'd', o să asume că nu
        if(rasp == 'd'){
            cout<<"La ce vagon doriți?"<<endl;
            cin>>wagon;
            if(cin.fail()){
                throw(-1);
                return;
            }
            if(wagon>12){ // asum aici maxim 12 vagoane
                throw(1);
                return;
            }

            cout<<"Ce loc doriți?"<<endl;
            cin>>seat;
            if(cin.fail()){
                throw(-1);
                return;
            }
            if((seat<0)||(seat>110)){ 
                throw(1);
                return;
            }
        }

        // acum că avem biletul, putem să-l memorăm în fișier
        vector<tuple<int,int,int,string>> ticket_list = get_ticket_list(ticket_file, username);

        // extracting current time
        time_t timestamp;
        time(&timestamp);
        string time_string = string(ctime(&timestamp));
        time_string = time_string.substr(0, time_string.length()-1);

        // aici puteam și fără tuplu, dar e mai comod să am toate componentele împreună
        new_ticket = make_tuple(ticket_class, wagon, seat, time_string);

        // TODO: să vezi dacă locurile alese sunt valide și să adaug locurile alea la train-itinerary (would be nice)

        // adaugă la finalul fișierului lista de bilete a utilizatorului
        ofstream ticket_output;
        ticket_output.open(ticket_file, ios_base::app);
        ticket_output<<get<0>(new_ticket)<<'-'<<get<1>(new_ticket)<<'-'<<get<2>(new_ticket)<<'-'<<get<3>(new_ticket)<<';';
        ticket_output<<endl;

        ticket_output.close();
    }
    
        
};


int main(){
    
   /*
    try{
        Login login_test("user_list.csv");
        login_test.login_user();
    }catch(...){
        cout<<"restul operațiilor nu pot fi realizate!"<<endl;
    }
    return 0;
    */
   //User test_user;
    char selection_char;
    while(selection_char!='c'&&selection_char!='o'){
        cout<<"Doriți să folosiți programul ca și călălor, sau ca și operator? (c - călător, o - operator)"<<endl;
        cin>>selection_char;
    }

    // TODO: catch exception
    if(selection_char=='c'){
        try{
            User session_user;
       
        
        selection_char='z';
        while(selection_char!='1'&&selection_char!='2'){
            cout<<"Selectați operația dorită: (1 - căutare cursă; 2 - rezervare bilet)"<<endl;
            cin>>selection_char;
        }
        
        if(selection_char=='1'){
            session_user.show_trains();
        }else if(selection_char=='2'){

            // în implementarea mea, evit puțin problema detaliilor introduse greșit, cum utilizatorul îți rezervă bilet în funcție de ID

            int train_id=0;
            cout<<"Introduceți ID-ul cursei dorite"<<endl;
            cin>>train_id;

            string temp_in;
            while(train_id==0){
                //cin.ignore();
                //cin.clear();
                cout<<"Textul introdus nu este un ID valid!\n";
                cout<<"Introduceți un ID:\n";
                cin>>temp_in;
                
                cin.clear();
                try{
                    train_id = stoi(temp_in);
                }catch(invalid_argument err){
                    cin.clear();
                    continue;
                }
                
            }

            session_user.buy_ticket(train_id);
        }

        }catch(int err){
            if(err==1){
                cout<<"Autentificare nereușită!"<<endl;
                
            }else{
                cout<<"Înregistrare nereușită!"<<endl;
            }
            return 0;
        }
    }else if(selection_char=='o'){
        try{ //aici am lăsat pt indentare ptc ar fi prea mult whitespace imo

        Operator session_operator;
        selection_char='z';
        while(selection_char!='1'&&selection_char!='2'){
            cout<<"Selectați operația dorită: (1 - adăugare cursă; 2 - ștergere cursă)"<<endl;
            cin>>selection_char;
        }

        if(selection_char=='1'){
            try{
                session_operator.add_itinerary();
            }catch(int err){
                if(err==-1){
                    cout<<"Caractere invalide!"<<endl;
                }
                cout<<"Cursa nu a putut fi adăugată!"<<endl;
                return 0;
            }
            
        }else if(selection_char=='2'){
            
            int train_id=0;
            cout<<"Introduceți ID-ul cursei de șters: "<<endl;
            cin>>train_id;

            string temp_in;
            while(train_id==0){
                //cin.ignore();
                //cin.clear();
                cout<<"Textul introdus nu este un ID valid!\n";
                cout<<"Introduceți un ID:\n";
                cin>>temp_in;
                
                cin.clear();
                try{
                    train_id = stoi(temp_in);
                }catch(invalid_argument err){
                    cin.clear();
                    continue;
                }
                
            }

            session_operator.remove_itinerary(train_id);
        }
         }catch(int err){
            if(err==1){
                cout<<"Autentificare nereușită!"<<endl;
                
            }else{
                cout<<"Înregistrare nereușită!"<<endl;
            }
            return 0;
        }
    }

    return 0;
}