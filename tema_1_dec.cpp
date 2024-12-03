#include <iostream>
#include <string>
#include <cctype>
#include <regex>
#include <fstream>
#include <set>
#include <utility>
#include <sstream>
using namespace std;

// clasa pt a parsa inputul de text
// preia numele fișierului și stochează liniile .csv-ului într-un vector de string

class CSV_input{
    public:

    vector<string> elements;

    CSV_input(string filename){
        ifstream fin(filename);
        cout<<filename<<endl;
        //fin.open(filename, ios::in);

        string line;

        if(fin.is_open()){
            cout<<"open"<<endl;
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
};

int TrainItinerary::global_id = 1;

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
    cout<<"Introduceți stația la care ajunge"<<endl;
    cin>>destination_temp;
    stations_temp.push_back(source_temp);
    
    // TODO: funcție pt a verifica dacă textul este valid (dacă e doar ASCII, nu caractere UTF)
    cout<<"Introduceți câte stații sunt între "<<source_temp<<" și "<<destination_temp<<endl;
    cin>>number_stations;
    string intermediary_station_temp;
    for(int i=1; i<=number_stations; i++){
        cout<<"Introduceți stația #"<<i<<endl;
        cin>>intermediary_station_temp;
        stations_temp.push_back(intermediary_station_temp);
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
        cout<<temp_attribute<<endl;
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
        cout<<temp_pair.first<<" "<<temp_pair.second<<endl;
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

// clasa operator
// TODO: alg de autentificare
class Operator{
    private:

    string username;
    // parola hash-uită
    string password;

    // string care conține numele fișierului de input/output
    string itinerary_file;
    vector<TrainItinerary> itinerary_arr;

    public:

    Operator(){
        // AICI INTRODUC PARTEA DE LOGIN
        // (o să pun în altă funcție partea de logare concretă, că pot să refolosesc din ea și la clasa de User)

        // pe asta o decomentez după ce am informații în fișier
        // ATENTIE: aici ar putea să meargă prost dacă VSCode nu deschide fișierul cu executabilul
        // dacă se deschide din alt fișier mai sus în ierarhie, nu citește bine
        itinerary_file = "train_itinerary.csv";

        itinerary_arr = get_itinerary_list(itinerary_file);
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
    
};

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


int main(){
    
    //int test = password_strength("Pecarenucare2212");
    
    int id = 1; //trb să găsesc mod de a genera ID
    string sursa = "București";
    string destinație = "Iași";
    vector<string> statii = {"București", "Focșani", "Iași"};
    string ora_plecare = "11:40";
    string ora_sosire = "15:00";
    string zi_plecare = "30/11/2024";
    string zi_sosire = "30/11/2024";
    
    vector<pair<int, int>> locuri_luate;
    locuri_luate.push_back(make_pair(4,47));
    locuri_luate.push_back(make_pair(5,31));
    locuri_luate.push_back(make_pair(1,63));

    TrainItinerary test(id,sursa,destinație,statii,ora_plecare,ora_sosire,zi_plecare,zi_sosire,locuri_luate);
    string test_string = test.write_to_line();
    cout<<test_string<<endl;
    
    TrainItinerary function_test = itinerary_from_string(test_string);
    cout<<function_test.write_to_line()<<endl;
    
    // ok programul nu dă crash dacă încerci să deschizi un fișier gol, asta e bine
    Operator operator_test = Operator();
    operator_test.add_itinerary();
    return 0;
}