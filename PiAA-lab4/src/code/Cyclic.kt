package code

fun main() {
    val a = readLine()!!
    val b = readLine()!!
    if (a.length != b.length) {
        println("-1")
    } else {
        val cyclic = a.isCyclic(b)
        println(cyclic)
    }
}

private fun String.prefix() : IntArray {
    val pi = IntArray(this.length)
    var k = 0
    for (q in 1 until this.length) {
        if (k > 0 && this[k] != this[q]) k = pi[k]
        if (this[k] == this[q]) k += 1
        pi[q] = k
    }
    return pi
}

fun String.isCyclic(other : String) : Int {
    val prf = other.prefix()
    var counter = 0
    for (i in 0 until this.length * 2) {
        while (counter > 0 && this[i % this.length] != other[counter]) counter = prf[counter - 1]
        if (this[i % this.length] == other[counter]) counter++
        if (counter == other.length) {
            return i - this.length + 1
        }
    }
    return -1
}
