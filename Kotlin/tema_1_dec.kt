// Cam mult spus "Tema 1 decembrie" at this point, dar nu contează
import java.io.File

class CSV_input(var filename: String){
    var elements = mutableListOf<String>()
    init{
        //elements = File(filename).useLines{it.toList()}
        // doamne e greu de descris în cuvinte cât de mult urăsc documentația de la Kotlin
        // trebuie să fac săpături arheologice ca să pot găsi librăria pe care trebuie să o importez
        File(filename).useLines { lines -> elements.addAll(lines) }
    }

}

fun main(){

    var test = CSV_input("test.csv")
}