/**
 * The class for building data by using Indexer
 * 
 * Execute:
 * args = {corpus file}
 * 
 * Class:
 * [BuildIndex]
 * Reader
 * TFIDFCalculator
 */

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;

import java.util.ArrayList;

import java.util.regex.Matcher;
import java.util.regex.Pattern;


public class BuildIndex {
    public static void main(String[] args) throws IOException {
        // args
        File corpusFile = new File(args[0]);
        String corpusFileName = corpusFile.getName().split("\\.")[0];

        // get document and parse
        DocumentReader source = null;
        DocumentSystem documentSystem = null;
        try {
            // args
            source = new DocumentReader(corpusFile);

            // read and parsing
            documentSystem = new DocumentSystem();
            while (true) {
                Document document = source.readDocument();
                if (document == null) {
                    break;
                }
                documentSystem.putDocument(document);
            }
        } catch (IOException e) {
            System.out.println("Can not read!");
        } finally {
            source.close();
        }
        

        // calculate TF-IDF and store it into Indexer
        Indexer indexer = new Indexer(corpusFileName);
        for (int number = 0; number < documentSystem.getDocumentCount(); number++) {
            Document document = documentSystem.getDocument(number);
            if (document == null) {
                continue;
            }
            for (String word : document.getWords()) {
                indexer.put(
                    word, 
                    number, 
                    TFIDFCalculator.TF_IDF_Calculate(word, document, documentSystem)
                );
            }
        }


        // serialize
        try {
            FileOutputStream fos = new FileOutputStream(corpusFileName + ".ser");
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(indexer);
            
            oos.close();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();	
        }
    }
}


class TFIDFCalculator {
    public static double TF(String term, Document document) {
        return (double) document.getWordCount(term) / document.getWordCount();
    }

    public static double IDF(String term, DocumentSystem documents) {
        return Math.log((double) documents.getDocumentCount() / documents.getWordCount(term));
    }
    

    public static double TF_IDF_Calculate(String term, Document document, DocumentSystem documents) {
        return TF(term, document) * IDF(term, documents);
    }
}


// document
/**
 * 管理文檔物件
 */
class DocumentSystem {
    /** 存所有的 Document物件 (文檔) */
    private ArrayList<Document> documents;

    /** @return 總文檔數 */
    public int getDocumentCount() {
        return documents.size();
    }

    
    /** 詞 : 幾個文檔內有 */
    private String2IntMap word2Count;

    /** @return 有出現 word 的文檔數 */
    public int getWordCount(String word) {
        return this.word2Count.get(word);
    }

    /** @return 取所有出現過的詞 */
    public ArrayList<String> getWords() {  // no usage
        return this.word2Count.keySet();
    }


    /** 純 new */
    public DocumentSystem() {
        this.documents = new ArrayList<>();
        this.word2Count = new String2IntMap();
    }


    /**
     * 把文檔放入管理系統內。
     * 遍歷文檔內的每個詞，把各詞的 出現次數 += 1
     * @param document 要放入的文檔
     */
    public void putDocument(Document document) {
        documents.add(document);  // TODO 如果 document 亂輸，documents 就不會照著 document number 排
        
        for (String word : document.getWords()) {
            word2Count.put(word);
        }
    }


    /**
     * @param number 要找的文檔的編號
     * @return 要找的文檔 if 有 else null
     */
    public Document getDocument(int number) {
        if (number >= getDocumentCount()) {
            return getDocumentCarefully(number);
        }

        Document document = this.documents.get(number);
        
        if (document.getNumber() != number) {
            return getDocumentCarefully(number);
        }
        
        return document;
    }
    
    /**
     * 仔細找，每個都看
     * @param number 要找的文檔的編號
     * @return 要找的文檔 if 有 else null
     */
    private Document getDocumentCarefully(int number) {
        for (Document document : this.documents) {
            if (document.getNumber() == number) {
                return document;
            }
        }
        return null;
    }
}

/**
 * 文檔物件
 */
class Document {
    /** 文檔編號 */
    private int number;

    /** @return 文檔編號 */
    public int getNumber() {
        return this.number;
    }


    /** 總詞數 */
    private int wordCount;

    /** @return 總詞數 */
    public int getWordCount() {
        return this.wordCount;
    }


    /** 詞 : 出現次數 */
    private String2IntMap word2Count;

    /** @return word 的出現次數 */
    public int getWordCount(String word) {
        return this.word2Count.get(word);
    }

    /** @return 取所有出現過的詞 */
    public ArrayList<String> getWords() {
        return this.word2Count.keySet();
    }


    /** 有存入詞，並排列 */
    public Document(ArrayList<String> words, int number) {
        this.number = number;
        this.wordCount = words.size();
        this.word2Count = new String2IntMap(words);
    }


    @Override
    public String toString() {
        return this.number + " : " + this.word2Count.keySet().toString() + " : " + this.wordCount;
    }
}


// Map
/**
 * 仿造 Map interface
 */
interface Mappable {
    /** 取一個 value */
    public int get(String key);
    
    /**
     * 放一對 key 和 value
     */
    public void put(String key, int value);

    /** 取所有的 key */
    public ArrayList<String> keySet();

    /** 取所有的 value */
    public ArrayList<Integer> values();

    /** 取得元素數 */
    public int size();
}

class String2IntMap implements Mappable {
    /** 用於快速查找詞，是一個map的構造 (詞 : 數量) */
    private Trie words;

    /** 詞集 (在建造DocumentSystem時才會用到) */
    private ArrayList<String> wordSet;


    /** 純 new */
    public String2IntMap() {
        this.words = new Trie();
        this.wordSet = new ArrayList<>();
    }

    /**
     * 初始化時就存入一堆詞
     * @param words 要存入的詞
     */
    public String2IntMap(ArrayList<String> words) {
        this();
        for (String str : words) {
            this.put(str);
        }
    }


    @Override
    public int get(String key) {
        return words.getCount(key);
    }


    /**
     * 把詞放入，並且把 出現次數 += value
     * @param key 要放入的詞
     * @param value 要增加的量
     */
    @Override
    public void put(String key, int value) {
        if (words.put(key, value) == false) {
            wordSet.add(key);
        }
    }

    /**
     * 把詞放入，並且把 出現次數 += 1
     * @param key 要放入的詞
     */
    public void put(String key) {
        this.put(key, 1);
    }


    @Override
    public ArrayList<String> keySet() {
        return this.wordSet;
    }


    @Override
    public ArrayList<Integer> values() {  // no usage
        ArrayList<Integer> output = new ArrayList<>(this.size());
        for (String key : wordSet) {
            output.add(words.getCount(key));
        }
        return output;
    }


    @Override
    public int size() {
        return wordSet.size();
    }
}


// trie
/**
 * 每個節點是一個 pair (詞 : 數量)
 */
class Trie {
    class TrieNode {
        /** next layer */
        private TrieNode[] children = new TrieNode[26];
        /** 此節點能不能是字的結尾 */
        private boolean isEndOfWord = false;
        /** value */
        private int count = 0;
    }


    private TrieNode[] root = new TrieNode[26];


    /** @return word 的出現次數 */
    public int getCount(String word) {
        TrieNode node = returnNode(word.charAt(0));
        if (node == null) {
            return 0;
        }

        for (int i = 1; i < word.length(); i++) {
            node = returnNode(node, word.charAt(i));
            if (node == null) {
                return 0;
            }
        }
        return node.count;
    }


    /**
     * 插入一個詞。
     * 若詞不存在，則 new TrieNode 和 count = value。
     * 若詞存在，則 count += value。
     * @param word 要插入的詞
     * @param value 要增加的量
     * @return 原先是否有這個詞 (false => word 原先不在裡面)
     */
    public boolean put(String word, int value) {
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
        if (node.count == 0) {  // check whether the word has been put in
            output = false;
            node.isEndOfWord = true;
        }
        
        node.count += value;
        return output;
    }

    /**
     * 插入一個詞。
     * 若詞不存在，則 new TrieNode 和 count = 1。
     * 若詞存在，則 count += 1。
     * @param word 要插入的詞
     * @return 是否有 new 新的節點 (true => word 原先不在裡面)
     */
    public boolean put(String word) {
        return this.put(word, 1);
    }
    

    /** 搜尋是否存在詞 */
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
}


// read
/**
 * 管理文字型檔案的讀取，可以關掉再重開
 * @provides readLine : 一次讀一行文字
 */
class Reader {
    protected BufferedReader reader = null;
    

    protected boolean readerOpenTF = false;
    public boolean hasOpen() {
        return this.readerOpenTF;
    }


    protected boolean fileEmptyTF = false;
    public boolean isEmpty() {
        return fileEmptyTF;
    }
    

    /**
     * @param path : 檔案路徑
     * @throws IOException
     */
    public Reader(String path) throws IOException {
        this(new File(path));
    }

    /**
     * @param file : 檔案物件
     * @throws IOException
     */
    public Reader(File file) throws IOException {
        this.open(file);
    }
    

    private void open(File file) throws IOException {
        this.fileEmptyTF = (file.length() == 0);
        // open
        this.reader = new BufferedReader(new FileReader(file));
        this.readerOpenTF = true;
    }


    /**
     * @return 一行文字
     * @throws IOException
     */
    public String readLine() throws IOException {
        return this.reader.readLine();
    }
    

    public void openNew(String path) throws IOException {
        this.openNew(new File(path));
    }

    public void openNew(File file) throws IOException {
        if (this.readerOpenTF == false) {
            this.open(file);
        }
    }
    

    public boolean close() throws IOException {
        if (this.readerOpenTF == true) {
            this.reader.close();
            this.readerOpenTF = false;
            this.fileEmptyTF = false;
            return true;
        }
        return false;  // does not close
    }
}

/**
 * 管理文檔讀取，可以關掉再重開
 * @provides readDocument : 一次讀一個文檔
 */
class DocumentReader extends Reader {
    public static final Pattern LINE_PATTERN = Pattern.compile("^(\\d+)\\t(.*)");
    public static final String SPLIT_BY = "[^a-z]+";

    /** 幾行代表一個文本 */
    public static final int DOCUMENT_LENGTH = 5;


    /** 目前讀取的文檔編號，readDocument()結束前要加一 */
    private int newDocumentNumber = 0;


    public DocumentReader(String path) throws IOException {
        super(path);
    }

    public DocumentReader(File file) throws IOException {
        super(file);
    }


    /**
     * @return Document 物件
     * @throws IOException
     */
    public Document readDocument() throws IOException {
        ArrayList<String> words = new ArrayList<>();  // store word from content

        for (int i = 0; i < DOCUMENT_LENGTH; i++) {  // TODO 行號應該不會亂跳，亂跳的話，會有問題
            String line = super.readLine();
            if (line == null) {  // end of file  // 直接不管已讀取的文本
                return null;
            }
            
            // get content
            Matcher contnetMatcher = LINE_PATTERN.matcher(line);
            if (!contnetMatcher.find()) {
                System.out.println("The pattern of \"" + line + "\" is illegal.");
                i--;
                continue;
            }
            String content = contnetMatcher.group(2);
            
            // get words
            content = content.toLowerCase();
            for (String str : content.split(SPLIT_BY)) {
                if (str != "") {
                    words.add(str);
                }
            }
        }

        // set document
        Document output = new Document(words, this.newDocumentNumber);

        this.newDocumentNumber += 1;
        return output;
    }


    @Override
    public boolean close() throws IOException {
        if (super.close()) {
            newDocumentNumber = 0;  // reset
            return true;
        }
        return false;
    }
}
