/**
 * The class for storing data
 * 
 * Class:
 * [Indexer]
 */

import java.io.Serializable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;


/**
 * 可序列化
 */
public class Indexer implements Serializable {
	private static final long serialVersionUID = 3L;


    /** corpus file name */
	private String parsingFileName;

    /** parsingFileName 的 getter */
    public String getParsingFileName() {
        return this.parsingFileName;
    }


    /** words */
    private transient ArrayList<String> __words = new ArrayList<>();
    /** words */
    private String[] words;

    /**
     * get index of word
     * <p>
     * if it does not exist => return the (-(index it should insert) - 1)
     */
    private int __getIndex(String word) {
        return Collections.binarySearch(this.__words, word);
    }

    /**
     * get index of word
     * <p>
     * if it does not exist => return the (-(index it should insert) - 1)
     */
    public int getIndex(String word) {
        return Arrays.binarySearch(this.words, word);
    }

    /**
     * check whether the word is in Indexer
     */
    public boolean hasWord(String word) {
        return getIndex(word) >= 0;
    }


    /** word to document numbers it has */
    private transient ArrayList<ArrayList<Integer>> __numbers = new ArrayList<>();
    /** word to document numbers it has */
    private int[][] numbers;

    /**
     * @param word the documentNumbers we want to get
     * @return the reference of ArrayList if the word in Indexer else []
     * @apiNote Do Not Change The Value In It
     */
    public int[] getNumbers(String word) {
        int index = getIndex(word);
        if (index >= 0) {
            return this.numbers[index];
        }
        else {
            return new int[0];
        }
    }

    /**
     * get index of documentNumber under specific word
     * <p>
     * if documentNumber does not exist => return (-(index it should insert) - 1)
     * @apiNote the word should in Indexer; otherwise, it will throw an exception
     */
    private int __getIndex(String word, int number) {
        return Collections.binarySearch(this.__numbers.get(__getIndex(word)), number);
    }

    /**
     * get index of documentNumber under specific word
     * <p>
     * if documentNumber does not exist => return (-(index it should insert) - 1)
     * @apiNote the word should in Indexer; otherwise, it will throw an exception
     */
    public int getIndex(String word, int number) {
        return Arrays.binarySearch(this.numbers[getIndex(word)], number);
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
    private transient ArrayList<ArrayList<Double>> __values = new ArrayList<>();
    /** word to values which depend on document numbers */
    private double[][] values;

    /**
     * @param word the values we want to get
     * @return the reference of ArrayList if the word in Indexer else []
     * @apiNote Do Not Change The Value In It
     */
    public double[] getValues(String word) {
        int index = getIndex(word);
        if (index >= 0) {
            return this.values[index];
        }
        else {
            return new double[0];
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
        int wordIndex = __getIndex(word);
        boolean output = wordIndex >= 0;

        if (output == false) {  // add new space for new word
            wordIndex += 1;
            wordIndex = -wordIndex;
            if (wordIndex < this.__words.size()) {
                this.__words.add(wordIndex, word);
                this.__numbers.add(wordIndex, new ArrayList<>());
                this.__values.add(wordIndex, new ArrayList<>());
            }
            else {  // add to end of the list
                this.__words.add(word);
                this.__numbers.add(new ArrayList<>());
                this.__values.add(new ArrayList<>());
                
            }
        }
        
        int documentIndex = __getIndex(word, documentNumber);
        
        if (documentIndex < 0) {  // add document's data: (number, value)
            documentIndex += 1;
            documentIndex = -documentIndex;
            
            if (wordIndex < this.__words.size()) {
                this.__numbers.get(wordIndex).add(documentIndex, documentNumber);
                this.__values.get(wordIndex).add(documentIndex, value);
            }
            else {  // add to end of the list
                this.__numbers.get(wordIndex).add(documentNumber);
                this.__values.get(wordIndex).add(value);
            }
        }
        else {
            this.__numbers.get(wordIndex).set(documentIndex, documentNumber);
            this.__values.get(wordIndex).set(documentIndex, value);
        }

        return output;
    }

    public void transform() {
        // words
        this.words = new String[__words.size()];
        for (int i = 0; i < __words.size(); i++) {
            words[i] = __words.get(i);
        }
        
        // numbers
        this.numbers = new int[__numbers.size()][];
        for (int i = 0; i < __numbers.size(); i++) {
            numbers[i] = new int[__numbers.get(i).size()];
            for (int j = 0; j < __numbers.get(i).size(); j++) {
                numbers[i][j] = __numbers.get(i).get(j);
            }
        }
        
        // values
        this.values = new double[__values.size()][];
        for (int i = 0; i < __values.size(); i++) {
            values[i] = new double[__values.get(i).size()];
            for (int j = 0; j < __values.get(i).size(); j++) {
                values[i][j] = __values.get(i).get(j);
            }
        }
    }
}
