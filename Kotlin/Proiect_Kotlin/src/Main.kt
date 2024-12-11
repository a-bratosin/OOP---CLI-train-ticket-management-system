// Cam mult spus "Tema 1 decembrie" at this point, dar nu contează
import java.io.ByteArrayInputStream
import java.io.File
import java.io.FileOutputStream
import java.nio.file.Paths
import java.util.UUID

// În principiu, țin să păstrez codul de Kotlin echivalent cu cel din C++, ceea ce înseamnă că structura și funcționalitatea este cam 90% la fel (chiar și funcționalitatea proastă!)
// Sunt totuși unele diferențe dintre cele două

// diferențe dintre versiunea de C++ și cea de Kotlin
// În loc de "Introduceți numărul de locuri disponibile:" am pus "Introduceți numărul de locuri ocupate:", cum trebuia
// Am modificat funcționalitatea de verificare a parolei, permițând introducerea de parole medii, fără caractere speciale


class WeakPasswordException(error_message: String): Exception(error_message)
class MediumPasswordException(error_message: String): Exception(error_message)
fun password_strength_factors(password: String){
    if(password.length<12){
        throw(WeakPasswordException("Parola este prea scurtă!"))
    }

    if(password==password.lowercase()){
        throw(WeakPasswordException("Parola nu conține majuscule!"))
    }

    if(password==password.uppercase()){
        throw(WeakPasswordException("Parola nu conține litere mici!"))
    }

    val num_regex = ".*[0-9].*".toRegex()
    if(!num_regex.matches(password)){
        throw(WeakPasswordException("Parola nu conține cifre!"))
    }

    val character_regex = "[!@#$%^&*()_=+{};:<>,./?]+".toRegex()
    if(!character_regex.matches(password)){
        throw(MediumPasswordException("Warning: Parola ar trebui să conțină și caractere speciale."))
    }
}

fun password_strength(password: String): Int{
    try{
        password_strength_factors(password)
    }catch(except: WeakPasswordException){
        println(except.message)
        return -1
    }catch(except:MediumPasswordException){
        println(except.message)
        return 0
    }

    return 1;
}

// idee proastă, dar e mult mai incomod cu UUID
object GlobalId {
    var value = 0;
    fun getNextInt(): Int = value++
}

class CSVInput(private var filename: String){
    var elements = mutableListOf<String>()
    init{
        //elements = File(filename).useLines{it.toList()}
        // doamne e greu de descris în cuvinte cât de mult urăsc documentația de la Kotlin
        // trebuie să fac săpături arheologice ca să pot găsi librăria pe care trebuie să o importez
        File(filename).useLines { lines -> elements.addAll(lines) }
    }

    fun write_line_to_file(new_line: String){
        //FileOutputStream pt a pune la final
        FileOutputStream(File(filename),true).bufferedWriter(Charsets.UTF_8).use { out -> out.write(new_line+"\n") }
    }

    fun write_file(){
        File(filename).bufferedWriter(Charsets.UTF_8).use { out -> out.write(elements.joinToString("\n")) }
    }

}

class TrainItinerary(
    private val id: Int,
    private val source:String,
    private val destination: String,
    private val stations: MutableList<String>,
    private val departure_time: String,
    private val departure_day: String,
    private val arrival_time: String,
    private val arrival_day: String,
    private val seats_taken: MutableList<Pair<Int,Int>>,
    ){

    fun write_to_line(): String{
        val output = StringBuilder()
        output.append(id)
            .append(",")
            .append(source)
            .append(",")
            .append(destination)
            .append(",")
        for(i in stations.indices){
            output.append(stations[i])
            if(i!=stations.size-1)
                output.append("-")
        }
        output.append(",")
            .append(departure_time)
            .append(",")
            .append(departure_day)
            .append(",")
            .append(arrival_time)
            .append(",")
            .append(arrival_day)
            .append(",")
        for(i in seats_taken.indices){
            output.append(seats_taken[i].first)
                .append(".")
                .append(seats_taken[i].second)
            if(i!=seats_taken.size-1)
                output.append("-")
        }

        return output.toString()
    }

    fun get_id(): Int{
        return this.id
    }

    fun get_departure_day(): String{
        return this.departure_day
    }
}

fun itinerary_from_keyboard(): TrainItinerary{
    var id_temp: Int = GlobalId.getNextInt()
    var stations_temp = mutableListOf<String>()
    var seats_taken_temp = mutableListOf<Pair<Int,Int>>()

    println("Introduceți stația din care pleacă cursa:")
    var source_temp: String = readln()
    println("Introduceți stația la care ajunge:")
    var destination_temp: String = readln()
    println("Introduceți câte stații sunt între $source_temp și $destination_temp")
    var number_stations: Int = readln().toInt()
    stations_temp.add(source_temp)
    for(i in 1..<number_stations){
        println("Introduceti stația ${i+1}")
        stations_temp.add(readln())
    }
    stations_temp.add(destination_temp)

    println("Introduceți ora de plecare: ")
    var departure_time_temp: String = readln()
    println("Introduceți ziua plecării: ")
    var departure_day_temp: String = readln()
    println("Introduceți ora de sosire: ")
    var arrival_time_temp: String = readln()
    println("Introduceți ziua sosirii: ")
    var arrival_day_temp: String = readln()

    println("Introduceți numărul de locuri ocupate:")
    var number_seats_taken: Int = readln().toInt()
    for(i in 1..<number_seats_taken){
        println("Introduceti perechea #${i+1} (vagon,loc): ")
        var wagon_temp = readln().toInt()
        var seat_temp = readln().toInt()
        seats_taken_temp.add(Pair(wagon_temp,seat_temp))
    }


    var output = TrainItinerary(id_temp, source_temp,destination_temp,stations_temp,departure_time_temp, departure_day_temp, arrival_time_temp, arrival_day_temp, seats_taken_temp)

    return output
}

private operator fun <E> List<E>.component6(): E = get(5)

private operator fun <E> List<E>.component7(): E = get(6)

private operator fun <E> List<E>.component8(): E = get(7)

private operator fun <E> List<E>.component9(): E = get(8)

fun itinerary_from_string(input: String): TrainItinerary{
    var input_stream = input.byteInputStream(Charsets.UTF_8).bufferedReader(Charsets.UTF_8)

    var (id_temp, source_temp, destination_temp, stations_temp, departure_time_temp, departure_day_temp, arrival_time_temp, arrival_day_temp, seats_taken_temp) = input.split(",", ignoreCase = false)

    var stations_temp_list: MutableList<String> = stations_temp.split("-", limit = 15).toMutableList()

    var seats_taken_pair = seats_taken_temp.split("-")
    var seats_taken_list = mutableListOf<Pair<Int,Int>>()
    for(pair_string in seats_taken_pair){
        val (wagon, seat) = pair_string.split(".")
        seats_taken_list.add(Pair(wagon.toInt(),seat.toInt()))
    }

    val output = TrainItinerary(id_temp.toInt(),source_temp,destination_temp,stations_temp_list, departure_time_temp, departure_day_temp, arrival_time_temp, arrival_day_temp, seats_taken_list)

    return output
}
fun get_itinerary_list(filename: String): MutableList<TrainItinerary>{
    var itinerary_arr = mutableListOf<TrainItinerary>()
    var input = CSVInput(filename)

    for(i in 0..<input.elements.size){
        itinerary_arr.add(itinerary_from_string(input.elements[i]))
    }


    // aici fac puțin diferit, căci păstrez valoarea din CSV, și apoi incrementez *înainte* să pun vreo intrare nouă
    if(itinerary_arr.size>0){
        GlobalId.value = itinerary_arr.last().get_id()
    }

    return itinerary_arr
}
fun main(){

    var test = CSVInput("test.csv")
    //test.write_line_to_file("add_this_three")
    //val path = Paths.get("").toAbsolutePath().toString()
    //println(path)
    test.elements.addLast("add_this_four")
    println(test.elements)
    test.write_file()
}