/**
 * The class for storing data
 * 
 * Class:
 * [Indexer]
 */

import java.io.Serializable;

import java.util.ArrayList;
import java.util.Collections;


/**
 * 可序列化
 */
public class Indexer implements Serializable {
	private static final long serialVersionUID = 2L;


    /** corpus file name */
	private String parsingFileName;

    /** parsingFileName 的 getter */
    public String getParsingFileName() {
        return this.parsingFileName;
    }


    /** words */
    private ArrayList<String> words = new ArrayList<>();

    /**
     * get index of word
     * <p>
     * if it does not exist => return the (-(index it should insert) - 1)
     */
    public int getIndex(String word) {
        return Collections.binarySearch(this.words, word);
    }

    /**
     * check whether the word is in Indexer
     */
    public boolean hasWord(String word) {
        return getIndex(word) >= 0;
    }


    /** word to document numbers it has */
    private ArrayList<ArrayList<Integer>> numbers = new ArrayList<>();

    /**
     * @param word the documentNumbers we want to get
     * @return the reference of ArrayList if the word in Indexer else []
     * @apiNote Do Not Change The Value In It
     */
    public ArrayList<Integer> getNumbers(String word) {
        int index = getIndex(word);
        if (index >= 0) {
            return this.numbers.get(index);
        }
        else {
            return new ArrayList<>();
        }
    }

    /**
     * get index of documentNumber under specific word
     * <p>
     * if documentNumber does not exist => return (-(index it should insert) - 1)
     * @apiNote the word should in Indexer; otherwise, it will throw an exception
     */
    public int getIndex(String word, int number) {
        return Collections.binarySearch(this.numbers.get(getIndex(word)), number);
    }

    /**
     * 1. check whether the word is in Indexer
     * <p>
     * 2. check whether the documentNumber is under the word
     */
    public boolean hasDocumentNumber(String word, int number) {
        boolean hasWord = this.hasWord(word);
        if (hasWord) {
            return getIndex(word, number) >= 0;
        }
        return false;
    }


    /** word to values which depend on document numbers */
    private ArrayList<ArrayList<Double>> values = new ArrayList<>();

    /**
     * @param word the values we want to get
     * @return the reference of ArrayList if the word in Indexer else []
     * @apiNote Do Not Change The Value In It
     */
    public ArrayList<Double> getValues(String word) {
        int index = getIndex(word);
        if (index >= 0) {
            return this.values.get(index);
        }
        else {
            return new ArrayList<>();
        }
    }
    

    /** new an Indexer with name */
    public Indexer(String fileName) {
        this.parsingFileName = fileName;
    }


    /**
     * put a word into Indexer
     * @param word the word we want to put
     * @param documentNumber the documentNumber we want to add under the word
     * @param value the value we want to set under the documentNumber
     * @return has the word been added to Indexer
     * @apiNote if insert the value again, the new value will replace previous value
     */
    public boolean put(String word, int documentNumber, double value) {  // In fact, the behavior is to insert.
        int wordIndex = getIndex(word);
        boolean output = wordIndex >= 0;

        if (output == false) {  // add new space for new word
            wordIndex += 1;
            wordIndex = -wordIndex;
            if (wordIndex < this.words.size()) {
                this.words.add(wordIndex, word);
                this.numbers.add(wordIndex, new ArrayList<>());
                this.values.add(wordIndex, new ArrayList<>());
            }
            else {  // add to end of the list
                this.words.add(word);
                this.numbers.add(new ArrayList<>());
                this.values.add(new ArrayList<>());
                
            }
        }
        
        int documentIndex = getIndex(word, documentNumber);
        
        if (documentIndex < 0) {  // add document's data: (number, value)
            documentIndex += 1;
            documentIndex = -documentIndex;
            
            if (wordIndex < this.words.size()) {
                this.numbers.get(wordIndex).add(documentIndex, documentNumber);
                this.values.get(wordIndex).add(documentIndex, value);
            }
            else {  // add to end of the list
                this.numbers.get(wordIndex).add(documentNumber);
                this.values.get(wordIndex).add(value);
            }
        }
        else {
            this.numbers.get(wordIndex).set(documentIndex, documentNumber);
            this.values.get(wordIndex).set(documentIndex, value);
        }

        return output;
    }
}
