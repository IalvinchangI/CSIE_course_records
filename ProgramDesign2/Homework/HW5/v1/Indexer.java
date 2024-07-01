/**
 * The class for storing data
 * 
 * Class:
 * [Indexer]
 * Indexer.TrieNode
 * Int2DoublePair
 */

import java.io.Serializable;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;


/**
 * Trie 類型，用來存各自的 TF、IDF
 * <p>
 * 可序列化
 */
public class Indexer implements Serializable {
	private static final long serialVersionUID = 1L;


    /** corpus file name */
	private String parsingFileName;

    /** parsingFileName 的 getter */
    public String getParsingFileName() {
        return this.parsingFileName;
    }
    

    /** Trie 的節點 */
    class TrieNode implements Serializable {
        /** next layer */
        private TrieNode[] children = new TrieNode[Indexer.ARRAY_LENGTH];


        /** 此節點能不能是字的結尾 */
        private boolean isEndOfWord = false;


        /** 各文檔的 TFIDF */
        private ArrayList<Int2DoublePair> tfidf = new ArrayList<>();
    }


    private static final int ARRAY_LENGTH = 26;


    private TrieNode[] root;


    public Indexer(String fileName) {
        this.parsingFileName = fileName;
        this.root = new TrieNode[Indexer.ARRAY_LENGTH];
    }


    public HashSet<Integer> getDocumentNumbers(String word) {
        HashSet<Integer> output = new HashSet<>();
        ArrayList<Int2DoublePair> allPair = getTFIDF(word);
        if (allPair == null) {
            return new HashSet<>();
        }
        for (Int2DoublePair pair : allPair) {
            output.add(pair.getNumber());
        }
        return output;
    }


    /** 
     * @apiNote 這沒有複製，改值時要記得先複製再改
     * @return word 的所有 number : TFIDF 對 if exist else null
     */
    public ArrayList<Int2DoublePair> getTFIDF(String word) {
        // search word
        TrieNode node = returnNode(word);
        if (node == null) {
            return null;  // the word does not in the indexer
        }
        return node.tfidf;
    }

    /** @return word 的 TFIDF */
    public double getTFIDF(String word, int documentNumber) {
        // search word
        TrieNode node = returnNode(word.charAt(0));
        if (node == null) {
            return 0;  // IDF = 0
        }

        for (int i = 1; i < word.length(); i++) {
            node = returnNode(node, word.charAt(i));
            if (node == null) {
                return 0;  // IDF = 0
            }
        }

        // search document
        int index = Collections.binarySearch(node.tfidf, new Int2DoublePair(documentNumber));
        if (index < 0) {
            return 0;  // TF = 0
        }

        return node.tfidf.get(index).getValue();  // TFIDF
    }
    

    /** 搜尋是否存在該詞 */
    public boolean search(String word) {  // no usage
        TrieNode node = returnNode(word.charAt(0));
        for (int i = 1; i < word.length(); i++) {
            node = returnNode(node, word.charAt(i));
            if (node == null) {
                return false;
            }
        }
        return node.isEndOfWord;
    }


    /**
     * 插入一個 word，並儲存 word 在 documentNumber 下的 TFIDF
     * 
     * @param word 要插入的詞
     * @param documentNumber 文檔名
     * @param tfidf 該詞的 TFIDF
     * @return 原先是否有這個詞 (false => word 原先不在裡面)
     */
    public boolean put(String word, int documentNumber, double tfidf) {
        boolean output = true;
        // root
        if (this.root[word.charAt(0) - 'a'] == null) {
            this.root[word.charAt(0) - 'a'] = new TrieNode();
        }
        TrieNode node = this.root[word.charAt(0) - 'a'];

        // children
        for (int i = 1; i < word.length(); i++) {
            if (node.children[word.charAt(i) - 'a'] == null) {
                node.children[word.charAt(i) - 'a'] = new TrieNode();
            }
            node = node.children[word.charAt(i) - 'a'];
        }

        // put
        if (node.isEndOfWord == false) {  // check whether the word has been put in
            output = false;
            node.isEndOfWord = true;
        }
        
        node.tfidf.add(new Int2DoublePair(documentNumber, tfidf));
        return output;
    }
    

    /**
     * 回傳 node 用的。
     * 只能用於讀。
     * @param current : 目前在的節點
     * @param key : 想找的節點
     * @return : null if 沒有節點 else TrieNode
     */
    private TrieNode returnNode(TrieNode current, char key) {
        return current.children[key - 'a'];
    }

    /**
     * 回傳 node 用的，只能找 root
     * 只能用於讀。
     * @param key : 想找的節點
     * @return : null if 沒有節點 else TrieNode
     */
    private TrieNode returnNode(char key) {
        return this.root[key - 'a'];
    }

    /**
     * 回傳 node 用的，能找到 word 所在的 node
     * 只能用於讀。
     * @param word : 想找的節點
     * @return : null if 沒有節點 else TrieNode
     */
    private TrieNode returnNode(String word) {
        TrieNode node = returnNode(word.charAt(0));
        if (node == null) {
            return null;
        }

        for (int i = 1; i < word.length(); i++) {
            node = returnNode(node, word.charAt(i));
            if (node == null) {
                return null;
            }
        }

        return node;
    }
}


/**
 * 一個 int 和 double 的資料對
 * <p>
 * 可比較
 */
class Int2DoublePair implements Serializable, Comparable<Int2DoublePair> {
    //** 文檔編號 */
    private int number;

    /** number 的 getter */
    public int getNumber() {
        return this.number;
    }


    /** 值 */
    private double value;

    /** value 的 getter */
    public double getValue() {
        return this.value;
    }

    /** value 的 setter */
    public void setValue(double value) {
        this.value = value;
    }

    /** this.value += value */
    public void addValue(double value) {
        this.value += value;
    }


    /**
     * 設定值
     * @param number : 文檔編號
     * @param value : 值
     */
    public Int2DoublePair(int number, double value) {
        this.number = number;
        this.value = value;
    }

    /**
     * 比較用的，不用輸入 value
     * @param number : 文檔編號
     */
    public Int2DoublePair(int number) {
        this.number = number;
    }


    @Override
    public int compareTo(Int2DoublePair o) {
        return this.number - o.number;
    }


    /** 回傳文檔編號 */
    @Override
    public String toString() {
        return Integer.toString(this.number);
    }
}
