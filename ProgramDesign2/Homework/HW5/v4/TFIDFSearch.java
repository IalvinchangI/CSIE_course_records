/**
 * for searching
 * 
 * Execute:
 * args = {corpus file name, testcase}
 * 
 * Class:
 * [TFIDFsearch]
 * Int2DoublePair
 * Writer
 */

import java.io.File;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.io.IOException;

import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.HashSet;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;


public class TFIDFSearch {
    public static final String AND = "\\s+AND\\s+";
    public static final String OR = "\\s+OR\\s+";
    
    public static final String OUTPUT_FILE_NAME = "output.txt";


    /**
     * 從 file 取出要找的詞和對應的文本
     * @param fileName 要讀取的 file
     * @param outQuery 回傳要 query
     * @return n
     * @throws IOException
     */
    public static int getQueryandN(String fileName, ArrayList<ArrayList<String>> outQuery) throws IOException {
        Reader reader = null;
        int n = 0;
        try {
            reader = new Reader(fileName);

            // get n
            n = Integer.parseInt(reader.readLine());

            String line = reader.readLine();
            while (line != null) {
                ArrayList<String> query = new ArrayList<>();
                String[] words = line.trim().split(AND);
                if (words.length != 1) {  // AND
                    query.add(AND);
                }
                else {
                    words = words[0].split(OR);
                    query.add(OR);
                }

                query.addAll(new HashSet<String>(Arrays.asList(words)));
                outQuery.add(query);
                line = reader.readLine();
            }
        } finally {
            reader.close();
            reader = null;
        }
        return n;
    }


    private static Object deserialize(String fileName) throws IOException {
        FileInputStream fis = null;
        ObjectInputStream ois = null;
        Object obj = null;
        try {
            fis = new FileInputStream(fileName + ".ser");
            ois = new ObjectInputStream(fis);
            obj = ois.readObject();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException c) {
            c.printStackTrace();
        } finally {
            ois.close();
            fis.close();
            ois = null;
            fis = null;
        }
        return obj;
    }


    /**
     * 把資料組成{@code Int2DoublePair}後，加進{@code output}
     */
    public static void addAllPairs(List<Int2DoublePair> output, int[] documentNumbers, double[] tfidfs) {
        for (int i = 0; i < documentNumbers.length; i++) {
            output.add(new Int2DoublePair(documentNumbers[i], tfidfs[i]));
        }
    }


    /**
     * 把資料組成{@code Int2DoublePair}後，根據{@code unionTF}決定模式，加進{@code output}
     */
    public static void addPairs(List<Int2DoublePair> output, int[] documentNumbers, double[] tfidfs, boolean unionTF) {
        int outputIndex = 0;
        int numbersIndex = 0;
        int tfidfsIndex = 0;
        while (true) {
            if (numbersIndex >= documentNumbers.length) {  // if run out of new data
                if (unionTF == false) {  // remove
                    for (; outputIndex < output.size(); outputIndex++) {
                        output.remove(outputIndex);
                    }
                }
                // we don't have to do anything when it is union
                break;
            }

            if (outputIndex >= output.size()) {  // if there are bigger number in numbers than in output
                if (unionTF == true) {  // add
                    while (numbersIndex < documentNumbers.length) {
                        output.add(new Int2DoublePair(documentNumbers[numbersIndex], tfidfs[tfidfsIndex]));
                        numbersIndex++;
                        tfidfsIndex++;
                    }
                }
                // we don't have to do anything when it is intersection
                break;
            }

            // normal
            if (output.get(outputIndex).number > documentNumbers[numbersIndex]) {  // number is not in output
                if (unionTF == true) {
                    output.add(outputIndex, new Int2DoublePair(documentNumbers[numbersIndex], tfidfs[tfidfsIndex]));
                    outputIndex++;  // move to original one(place)
                }
                // we don't have to do anything when it is intersection
                numbersIndex++;  // get next data
                tfidfsIndex++;  // get next data
            }
            else if (output.get(outputIndex).number < documentNumbers[numbersIndex]) {  // it's not the place to insert pair or the number may appear later
                if (unionTF == false) {
                    output.remove(outputIndex);
                }
                else {  // we don't have to do anything when it is union
                    outputIndex++;  // move to next one
                }
            }
            else {  // (output.get(outputIndex).number == number)
                output.get(outputIndex).value += tfidfs[tfidfsIndex];
                numbersIndex++;  // get next data
                tfidfsIndex++;  // get next data
                outputIndex++;  // move to next one
            }
        }
    }


    public static void write(List<Int2DoublePair> data, int n, Writer writer) throws IOException {
        Collections.sort(data, Int2DoublePair.pairComparator);

        Iterator<Int2DoublePair> dataIterator = data.iterator();
        for (int i = 0; i < n; i++) {
            if (dataIterator.hasNext()) {
                writer.write(dataIterator.next().toString());
            }
            else {
                writer.write("-1");
            }

            if (i != (n - 1)) {
                writer.write(" ");
            }
            else {
                writer.write("\n");
            }
        }
    }


    public static void main(String[] args) throws IOException {
        long start = System.currentTimeMillis();
        // args
        String serFileName = args[0];

        // deserialize
        IndexerContainer indexerContainer = new IndexerContainer(serFileName);
        String[] allWords = (String[]) deserialize(serFileName);

        // args
        ArrayList<ArrayList<String>> queries = new ArrayList<>();
        int n = getQueryandN(args[1], queries);

        // search
        Writer writer = new Writer(OUTPUT_FILE_NAME);
        try {
            for (ArrayList<String> query : queries) {
                ArrayList<Int2DoublePair> result = new ArrayList<>();
                boolean unionTF = query.get(0).equals(OR);
                
                // deserialize
                for (int i = 1; i < query.size(); i++) {
                    if ((Arrays.binarySearch(allWords, query.get(i)) >= 0) && (!indexerContainer.hasWord(query.get(i)))) {
                        Indexer indexer = (Indexer) deserialize(serFileName + "_" + query.get(i));
                        indexerContainer.put(query.get(i), indexer.numbers, indexer.values);
                    }
                }

                addAllPairs(result, indexerContainer.getNumbers(query.get(1)), indexerContainer.getValues(query.get(1)));
                for (int i = 2; i < query.size(); i++) {
                    addPairs(
                        result, 
                        indexerContainer.getNumbers(query.get(i)), indexerContainer.getValues(query.get(i)), 
                        unionTF
                    );
                }
                
                write(result, n, writer);
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            writer.close();
        }
        long end = System.currentTimeMillis();
        System.out.println((end - start) / 1000.);
    }
}



/**
 * 一個 int 和 double 的資料對
 * <p>
 * 可比較
 */
class Int2DoublePair implements Comparable<Int2DoublePair> {
    public static Comparator<Int2DoublePair> pairComparator = new Comparator<Int2DoublePair>() {
        @Override
        public int compare(Int2DoublePair o1, Int2DoublePair o2) {
            // tfidf large -> small
            if (o1.value > o2.value) {
                return -1;
            }
            if (o1.value < o2.value) {
                return 1;
            }
            // number small -> large
            if (o1.number > o2.number) {
                return 1;
            }
            if (o1.number < o2.number) {
                return -1;
            }
            return 0;
        } 
    };


    //** 文檔編號 */
    public int number;


    /** 值 */
    public double value;


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



// write
/**
 * 管理文字型檔案的編寫，可以關掉再重開
 * @provides write : 寫入文字
 */
class Writer {
    protected BufferedWriter writer = null;
    

    protected boolean writerOpenTF = false;
    public boolean hasOpen() {
        return this.writerOpenTF;
    }


    protected boolean fileEmptyTF = false;
    public boolean isEmpty() {
        return fileEmptyTF;
    }
    

    /**
     * @param path : 檔案路徑
     * @param appendTF : 是否要用append的方式編寫
     * @throws IOException
     */
    public Writer(String path, boolean appendTF) throws IOException {
        this(new File(path), appendTF);
    }

    /**
     * @param path : 檔案路徑
     * @throws IOException
     */
    public Writer(String path) throws IOException {
        this(new File(path));
    }

    /**
     * @param file : 檔案物件
     * @throws IOException
     */
    public Writer(File file) throws IOException {
        this(file, false);
    }

    /**
     * @param file : 檔案物件
     * @param appendTF : 是否要用append的方式編寫
     * @throws IOException
     */
    public Writer(File file, boolean appendTF) throws IOException {
        this.open(file, appendTF);
    }
    

    private void open(File file, boolean appendTF) throws IOException {
        file.createNewFile();  // create file if the file does not exist

        this.fileEmptyTF = (file.length() == 0);
        // open
        this.writer = new BufferedWriter(new FileWriter(file, appendTF));
        this.writerOpenTF = true;
    }


    /**
     * 寫入文字，不會自動換行
     * @param s : 要寫入的字句
     * @throws IOException
     */
    public void write(String s) throws IOException {
        this.writer.write(s);
    }
    

    public void openNew(String path, boolean appendTF) throws IOException {
        this.openNew(new File(path), appendTF);
    }

    public void openNew(File file, boolean appendTF) throws IOException {
        if (this.writerOpenTF == false) {
            this.open(file, appendTF);
        }
    }
    

    public void close() throws IOException {
        if (this.writerOpenTF == true) {
            this.writer.close();
            this.writerOpenTF = false;
            this.fileEmptyTF = false;
        }
    }
}
