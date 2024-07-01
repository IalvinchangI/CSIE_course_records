/**
 * for searching
 * 
 * Execute:
 * args = {corpus file name, testcase}
 * 
 * Class:
 * [TFIDFsearch]
 * Writer
 */

import java.io.File;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.io.IOException;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.LinkedList;


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

                query.addAll(Arrays.asList(words));
                outQuery.add(query);
                line = reader.readLine();
            }
        } finally {
            reader.close();
        }
        return n;
    }


    public static void write(LinkedList<Int2DoublePair> data, int n, Writer writer) throws IOException {
        Collections.sort(data, new Comparator<Int2DoublePair>() {
            @Override
            public int compare(Int2DoublePair o1, Int2DoublePair o2) {
                // tfidf large -> small
                if (o1.getValue() > o2.getValue()) {
                    return -1;
                }
                if (o1.getValue() < o2.getValue()) {
                    return 1;
                }
                // number small -> large
                if (o1.getNumber() > o2.getNumber()) {
                    return 1;
                }
                if (o1.getNumber() < o2.getNumber()) {
                    return -1;
                }
                return 0;
            } 
        });

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
        }
        long end = System.currentTimeMillis();
        System.out.println((end - start) / 1000.);

        // args
        ArrayList<ArrayList<String>> queries = new ArrayList<>();
        int n = getQueryandN(args[1], queries);

        // search
        start = System.currentTimeMillis();
        Writer writer = new Writer(OUTPUT_FILE_NAME);
        try {
            for (ArrayList<String> query : queries) {
                LinkedList<Int2DoublePair> result = new LinkedList<>();
                if (query.size() == 1) {
                    result.addAll(indexer.getTFIDF(query.get(0)));
                    write(result, n, writer);
                    continue;
                }

                HashSet<Integer> numberSet = new HashSet<>();
                if (query.get(0).equals(AND)) {
                    numberSet.addAll(indexer.getDocumentNumbers(query.get(1)));
                    for (int i = 2; i < query.size(); i++) {
                        numberSet.retainAll(indexer.getDocumentNumbers(query.get(i)));
                    }
                }
                else if (query.get(0).equals(OR)) {
                    for (int i = 1; i < query.size(); i++) {
                        numberSet.addAll(indexer.getDocumentNumbers(query.get(i)));
                    }
                }
                else {  // skip
                    continue;
                }

                for (int number : numberSet) {
                    Int2DoublePair pair = new Int2DoublePair(number, indexer.getTFIDF(query.get(1), number));
                    for (int i = 2; i < query.size(); i++) {
                        pair.addValue(indexer.getTFIDF(query.get(i), number));
                    }
                    result.add(pair);
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
