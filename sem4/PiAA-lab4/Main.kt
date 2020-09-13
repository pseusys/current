package code

private data class ListAndInt(var list: List<Int>, var int: Int)

fun main() {
    val mark = readLine()!!
    val base = readLine()!!
    val entryArray = base.findAllSync(mark)
    println(if (entryArray.isEmpty()) "-1" else entryArray.joinToString(","))
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

private fun String.findAll(mark : String, prf : IntArray, offset : Int) : ListAndInt {
    val answer = mutableListOf<Int>()
    var counter = 0
    for (i in this.indices) {
        while (counter > 0 && this[i] != mark[counter]) counter = prf[counter - 1]
        if (this[i] == mark[counter]) {
            if (counter == 0) println("Entry found at pos ${i + offset + 1}")
            if (counter == mark.length - 1) println("Entry confirmed!")
            counter++
        }
        if (counter == mark.length) {
            answer.add(i - mark.length + 1)
            counter = prf[counter - 1]
        }
    }
    return ListAndInt(answer, counter)
}

fun String.findAllSync(mark : String) : IntArray {
    val prefixes = mark.prefix()
    return if (mark.length > this.length / 100) {
        findAll(mark, prefixes, 0).list.toIntArray()
    } else {
        val ptL = mark.length * 100
        val answer = mutableListOf<Int>()
        for (i in 0 until (this.length / ptL)) {
            var subs = this.substring(i * ptL, (i + 1) * ptL)
            var result = subs.findAll(mark, prefixes, i * ptL)
            answer.addAll(result.list.map { it + i * ptL })
            val offset = result.int
            val tailLen = offset + mark.length - 1
            subs = this.substring((i + 1) * ptL - offset, ((i + 1) * ptL - offset + tailLen).coerceAtMost(this.length))
            result = subs.findAll(mark, prefixes, (i + 1) * ptL - offset)
            answer.addAll(result.list.map { it + i * ptL + ptL - offset })
        }
        val result = this.substring(this.length - (this.length % ptL)).findAll(mark, prefixes, this.length - (this.length % ptL))
        answer.addAll(result.list.map { it + this.length - (this.length % ptL) })
        answer.toIntArray()
    }
}
