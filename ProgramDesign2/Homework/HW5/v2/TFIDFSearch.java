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


    /**
     * 把資料組成{@code Int2DoublePair}後，加進{@code output}
     */
    public static void addAllPairs(List<Int2DoublePair> output, List<Integer> documentNumbers, List<Double> tfidfs) {
        for (int i = 0; i < documentNumbers.size(); i++) {
            output.add(new Int2DoublePair(documentNumbers.get(i), tfidfs.get(i)));
        }
    }


    /**
     * 把資料組成{@code Int2DoublePair}後，根據{@code unionTF}決定模式，加進{@code output}
     */
    public static void addPairs(List<Int2DoublePair> output, List<Integer> documentNumbers, List<Double> tfidfs, boolean unionTF) {
        int outputIndex = 0;
        Iterator<Integer> numbersIterator = documentNumbers.iterator();
        Iterator<Double> tfidfsIterator = tfidfs.iterator();
        int number = -1;
        double tfidf = -1;
        boolean iterateNextTF = true;
        while (true) {
            if (iterateNextTF) {  // ask for next data
                if (numbersIterator.hasNext() == false) {  // end of new data
                    if (unionTF == false) {  // remove
                        for (; outputIndex < output.size(); outputIndex++) {
                            output.remove(outputIndex);
                        }
                    }
                    // we don't have to do anything when it is union
                    break;
                }
                // has data
                number = numbersIterator.next();
                tfidf = tfidfsIterator.next();
                iterateNextTF = false;  // we temporarily do not get next data
            }

            if (outputIndex >= output.size()) {  // there are bigger number in numbers than in output
                if (unionTF == true) {  // add
                    output.add(new Int2DoublePair(number, tfidf));
                    while (numbersIterator.hasNext()) {
                        output.add(new Int2DoublePair(
                                                numbersIterator.next(), 
                                                tfidfsIterator.next()
                        ));
                    }
                }
                // we don't have to do anything when it is intersection
                break;
            }

            if (output.get(outputIndex).number > number) {  // number is not in output
                if (unionTF == true) {
                    output.add(outputIndex, new Int2DoublePair(number, tfidf));
                    outputIndex++;  // move to original one
                }
                // we don't have to do anything when it is intersection
                iterateNextTF = true;  // get next data
            }
            else if (output.get(outputIndex).number < number) {  // it's not the place to insert pair or the number may appear later
                if (unionTF == false) {
                    output.remove(outputIndex);
                }
                else {  // we don't have to do anything when it is union
                    outputIndex++;  // move to next one
                }
            }
            else {  // (output.get(outputIndex).number == number)
                output.get(outputIndex).value += tfidf;
                iterateNextTF = true;  // get next data
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
        // args
        String fullSerFileName = args[0] + ".ser";

        // deserialize
        long start = System.currentTimeMillis();
        Indexer indexer = null;
        FileInputStream fis = null;
        ObjectInputStream ois = null;
        try {
            fis = new FileInputStream(fullSerFileName);
            ois = new ObjectInputStream(fis);
            indexer = (Indexer) ois.readObject();
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
        long end = System.currentTimeMillis();
        System.out.println((end - start) / 1000.);


        start = System.currentTimeMillis();
        // args
        ArrayList<ArrayList<String>> queries = new ArrayList<>();
        int n = getQueryandN(args[1], queries);

        // search
        Writer writer = new Writer(OUTPUT_FILE_NAME);
        try {
            for (ArrayList<String> query : queries) {
                ArrayList<Int2DoublePair> result = new ArrayList<>();
                boolean unionTF = query.get(0).equals(OR);
                
                addAllPairs(result, indexer.getNumbers(query.get(1)), indexer.getValues(query.get(1)));
                for (int i = 2; i < query.size(); i++) {
                    addPairs(
                        result, 
                        indexer.getNumbers(query.get(i)), indexer.getValues(query.get(i)), 
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
        end = System.currentTimeMillis();
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
