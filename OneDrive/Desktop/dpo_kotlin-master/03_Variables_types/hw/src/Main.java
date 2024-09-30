public class Main {
    fun main(args: Array<String>) {
        val firstName: String = "Зара"
        val lastName: String = "Серобян"
        var height: Double = 1.65
        var weight: Float = 50

        var isChild: Boolean = height < 1.5 && weight < 40
        var info = firstName + " " + lastName + " " + height + " " + weight
        printLn(info)