// jsoup
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

// io
import java.io.IOException;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.text.DecimalFormat;

// data struct
import java.util.Collections;
import java.util.ArrayList;
import java.util.HashMap;




public class HtmlParser {
    public static final String URL = "https://pd2-hw3.netdb.csie.ncku.edu.tw/";
    public static final String DATA_FILE_NAME = "data.csv";
    public static final String OUTPUT_FILE_NAME = "output.csv";

    public static final int SIMPLE_MOVING_AVERAGE_TIME_LENGTH = 5;

    public static void main(String[] args) {
        int mode = Integer.parseInt(args[0]);
        CsvWriter dataCSV = null;
        try {
            switch (mode) {
                case 0:  // crawl
                    DataGetter crawler = new DataGetter(URL);  // crawler = URL version's DataGetter
                    dataCSV = new CsvWriter(DATA_FILE_NAME, true);  // mode = append
                    if (dataCSV.isEmpty() == true) {
                        dataCSV.write("DAY", ",");  // add one more column for day; String version
                        dataCSV.write(crawler.getStocks());  // append title if file is empty; Iterable version
                    }
                    dataCSV.write(crawler.getStringDay(), ",");  // append day first; String version
                    dataCSV.write(crawler.getStringDayDatas());  // append data for current day; Iterable version
                    dataCSV.close();
                    break;
                case 1:  // parse
                    int task = Integer.parseInt(args[1]);
                    if (task == 0) {  // just print out
                        DataGetter fileReader = new DataGetter(new File(DATA_FILE_NAME));  // fileReader = FILE version's DataGetter
                        dataCSV = new CsvWriter(OUTPUT_FILE_NAME);  // mode = write
                        dataCSV.write(fileReader.getStocks());  // write title first; Iterable version
                        for (int day : fileReader.getDays()) {
                            dataCSV.write(fileReader.getStringDayDatas(day));  // write data for every day; Iterable version
                        }
                        dataCSV.close();
                    } else {  // compute and output
                        String stock = args[2].toUpperCase();
                        int start = Integer.parseInt(args[3]);
                        int end   = Integer.parseInt(args[4]);
                        parse(task, stock, start, end);
                    }
                    break;
                default:
                    throw new IllegalArgumentException("mode" + mode + " is not expected.");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void parse(int task, String stockName, int startTime, int endTime) throws IOException {
        switch (task) {
            case 1:
                taskForSimpleMovingAverage(stockName, startTime, endTime);
                break;
            case 2:
                taskForStandardDeviation(stockName, startTime, endTime);
                break;
            case 3:
                taskForTop3StandardDeviation(startTime, endTime);
                break;
            case 4:
                taskForRegressionLine(stockName, startTime, endTime);
                break;
            default:
                throw new IllegalArgumentException("task" + task + " is not expected.");
        }
    }

    private static void taskForSimpleMovingAverage(String stockName, int startTime, int endTime) throws IOException {
        DataGetter fileReader = new DataGetter(new File(DATA_FILE_NAME));  // fileReader = FILE version's DataGetter; only read "stockName"
        CsvWriter dataCSV = new CsvWriter(OUTPUT_FILE_NAME, true);  // mode = append

        dataCSV.write(new String[]{stockName, Integer.toString(startTime), Integer.toString(endTime)});  // write title; Iterable version

        ArrayList<String> ouput = new ArrayList<String>();
        for (int day = startTime; day <= endTime - SIMPLE_MOVING_AVERAGE_TIME_LENGTH + 1; day++) {  // +1 : include endTime
            ouput.add(ListMath.double2String(
                ListMath.average(fileReader.getStockDatas(stockName, day, day + SIMPLE_MOVING_AVERAGE_TIME_LENGTH))
            ));
        }
        dataCSV.write(ouput);  // Iterable version

        dataCSV.close();
    }
    private static void taskForStandardDeviation(String stockName, int startTime, int endTime) throws IOException {
        DataGetter fileReader = new DataGetter(new File(DATA_FILE_NAME));  // fileReader = FILE version's DataGetter; only read "stockName"
        CsvWriter dataCSV = new CsvWriter(OUTPUT_FILE_NAME, true);  // mode = append

        dataCSV.write(new String[]{stockName, Integer.toString(startTime), Integer.toString(endTime)});  // write title; Iterable version

        dataCSV.write(ListMath.double2String(
            ListMath.standardDeviation(fileReader.getStockDatas(stockName, startTime, endTime + 1))  // Double version
        ), "\n");

        dataCSV.close();
    }
    private static void taskForTop3StandardDeviation(int startTime, int endTime) throws IOException {
        DataGetter fileReader = new DataGetter(new File(DATA_FILE_NAME));  // fileReader = FILE version's DataGetter
        CsvWriter dataCSV = new CsvWriter(OUTPUT_FILE_NAME, true);  // mode = append
        final int TOP3 = 3;

        // get every pair of (stockName and standardDeviation)
        ArrayList<ArrayList<Object>> pairs = new ArrayList<ArrayList<Object>>();
        for (String stockName : fileReader.getStocks()) {
            ArrayList<Object> pair = new ArrayList<Object>();
            pair.add(stockName);                                                                                // 0 = String
            pair.add(ListMath.standardDeviation(fileReader.getStockDatas(stockName, startTime, endTime + 1)));  // 1 = double
            pairs.add(pair);
        }

        // sort and get Top3
        Collections.sort(pairs, (a1, a2) -> {
            double d1 = (double) a1.get(1);
            double d2 = (double) a2.get(1);
            if (d1 > d2) {
                return -1;
            } else if (d1 < d2) {
                return 1;
            } else {
                return 0;
            }
        });

        ArrayList<String> ouputTop3Name = new ArrayList<String>();
        ArrayList<String> ouputTop3Value = new ArrayList<String>();
        for (int i = 0; i < TOP3; i++) {
            ouputTop3Name.add((String) pairs.get(i).get(0));
            ouputTop3Value.add(ListMath.double2String(
                (double) pairs.get(i).get(1)
            ));
        }
        
        // write title
        dataCSV.write(ouputTop3Name, ",");  // Iterable version
        dataCSV.write(new String[]{Integer.toString(startTime), Integer.toString(endTime)});  // Iterable version
        dataCSV.write(ouputTop3Value);  // Iterable version

        dataCSV.close();
    }
    private static void taskForRegressionLine(String stockName, int startTime, int endTime) throws IOException {
        DataGetter fileReader = new DataGetter(new File(DATA_FILE_NAME));  // fileReader = FILE version's DataGetter; only read "stockName"
        CsvWriter dataCSV = new CsvWriter(OUTPUT_FILE_NAME, true);  // mode = append

        dataCSV.write(new String[]{stockName, Integer.toString(startTime), Integer.toString(endTime)});  // write title; Iterable version

        dataCSV.write(ListMath.double2String(
            ListMath.regressionLine(
                ListMath.rangeDouble(startTime, endTime + 1), fileReader.getStockDatas(stockName, startTime, endTime + 1)
            )  // Iterable version (slope, intercept)
        ));
        dataCSV.close();
    }
}



class DataGetter {
    private HashMap<String, Stock> stock2Datas = null;
    public ArrayList<Double> getDayDatas(int day) {
        ArrayList<Double> output = new ArrayList<Double>();
        for (String stockName : stockOrder) {
            try {
                output.add(
                    this.stock2Datas.get(stockName).getPrice(day)  // normal
                );
            } catch (IndexOutOfBoundsException e) {
                output.add(0d);  // add 0 if day does not in the key set of day2Price
            }
        }
        return output;
    }
    public ArrayList<Double> getDayDatas() {
        return this.getDayDatas(this.getDay());
    }
    public ArrayList<String> getStringDayDatas(int day) {
        ArrayList<String> output = new ArrayList<String>();
        for (String stockName : stockOrder) {
            output.add(
                String.format("%.2f", this.stock2Datas.get(stockName).getPrice(day))
            );
        }
        return output;
    }
    public ArrayList<String> getStringDayDatas() {
        return this.getStringDayDatas(this.getDay());
    }
    public ArrayList<Double> getStockDatas(String name) {
        return this.stock2Datas.get(name).getPrices(this.getDays());
    }
    public ArrayList<Double> getStockDatas(String name, int start, int end) {
        return this.stock2Datas.get(name).getPrices(start, end);
    }

    private ArrayList<String> stockOrder = null;
    public ArrayList<String> getStocks() {
        return this.stockOrder;
    }
    public String getStock(int index) {
        return this.stockOrder.get(index);
    }

    private ArrayList<Integer> days = null;
    public int getDay(int index) {
        return this.days.get(index);
    }
    public int getDay() {
        return this.getDay(0);
    }
    public ArrayList<Integer> getDays() {  // sorted
        ArrayList<Integer> output = new ArrayList<Integer>(this.days);
        Collections.sort(output);
        return output;
    }
    public String getStringDay(int index) {
        return Integer.toString(this.days.get(index));
    }
    public String getStringDay() {
        return this.getStringDay(0);
    }
    public ArrayList<String> getStringDays() {  // sorted
        ArrayList<String> output = new ArrayList<String>();
        for (int day : this.getDays()) {
            output.add(Integer.toString(day));
        }
        return output;
    }

    protected DataGetter() {
        this.stock2Datas = new HashMap<String, Stock>();
        this.stockOrder = new ArrayList<String>();
        this.days = new ArrayList<Integer>();
    }
    public DataGetter(String url) throws IOException {
        this();
        Document document = Jsoup.connect(url).get();
        Elements tableRow = document.select("body table tbody tr");

        // get day
        int day = Integer.parseInt(document.title().substring(3));
        days.add(day);

        // get stock and price
        Elements stocks = tableRow.eq(0).select("th");
        Elements prices = tableRow.eq(1).select("td");
        if (stocks.size() != prices.size()) {
            throw new IllegalArgumentException("stocks.size != prices.size (" + stocks.size() + ", " + prices.size() + ")");
        }
        for (int i = 0; i < stocks.size(); i++) {
            String stockName = stocks.get(i).text().toUpperCase();
            double price = Double.parseDouble(prices.get(i).text());

            this.stockOrder.add(stockName);  // store stock
            // store price
            this.stock2Datas.put(stockName, new Stock(stockName));
            this.stock2Datas.get(stockName).addPrice2Day(day, price);
        }
    }
    public DataGetter(File file) throws IOException {
        this();
        CsvReader dataCSV = new CsvReader(file);
        
        // stock name
        String[] stockNames = dataCSV.readLineSplit();
        for (int i = 1; i < stockNames.length; i++) {
            String stockName = stockNames[i].toUpperCase();
            this.stockOrder.add(stockName);
            this.stock2Datas.put(stockName, new Stock(stockName));  // initialize Stock
        }
        
        // price for each day
        String[] dayAndPrices = null;
        while ((dayAndPrices = dataCSV.readLineSplit()) != null) {
            if (dayAndPrices.length == 1 && dayAndPrices[0].equals("")) {  // this line is empty
                continue;
            }
            // day
            int day = Integer.parseInt(dayAndPrices[0]);  // first element in the row is day
            if (this.days.contains(day) == true) {
                continue;  // skip this row
            }
            this.days.add(day);
        
            // price
            if (this.stockOrder.size() != (dayAndPrices.length - 1)) {
                throw new IllegalArgumentException("stocks.size != prices.size (" + this.stockOrder.size() + ", " + (dayAndPrices.length - 1) + ")");
            }
            int i = 1;
            for (String stockName : this.stockOrder) {
                this.stock2Datas.get(stockName).addPrice2Day(day, Double.parseDouble(dayAndPrices[i]));
                i++;
            }
        }
        
        dataCSV.close();
    }
}

class Stock {
    private String name = null;
    public String getName() {
        return this.name;
    }

    private HashMap<Integer, Double> day2Price = null;
    public double getPrice(int day) throws IndexOutOfBoundsException {
        if (day2Price.containsKey(day)) {
            return this.day2Price.get(day);
        } else {
            throw new IndexOutOfBoundsException();
        }
    }
    public ArrayList<Double> getPrices(ArrayList<Integer> list) {
        ArrayList<Double> output = new ArrayList<Double>();
        for (int day : list) {
            try {
                output.add(this.getPrice(day));  // normal
            } catch (IndexOutOfBoundsException e) {
                output.add(0d);  // add 0 if day does not in the key set of day2Price
            }
        }
        return output;
    }
    public ArrayList<Double> getPrices(int start, int end) {
        ArrayList<Double> output = new ArrayList<Double>();
        for (int day = start; day < end; day++) {  // exclude end
            try {
                output.add(this.getPrice(day));  // normal
            } catch (IndexOutOfBoundsException e) {
                output.add(0d);  // add 0 if day does not in the key set of day2Price
            }
        }
        return output;
    }
    public void addPrice2Day(int day, double price) {
        this.day2Price.put(day, price);
    }

    public Stock(String name) {
        this.name = name;
        this.day2Price = new HashMap<Integer, Double>();
    }

}

final class ListMath {
    public static final DecimalFormat DECIMAL_FORMAT = new DecimalFormat("0.##");
    public static final int DECIMAL_POINT_PRECISION = 2;
    public static final double PRECISION = power(0.1, DECIMAL_POINT_PRECISION + 4);

    public static String double2String(double d) {
        double shift = power(10, DECIMAL_POINT_PRECISION);
        long temp_d = (long)(d * shift + ((d >= 0) ? 0.5 : -0.5));
        return DECIMAL_FORMAT.format(temp_d / shift);
    }
    public static ArrayList<String> double2String(ArrayList<Double> list) {
        ArrayList<String> output = new ArrayList<String>(list.size());
        for (double d : list) {
            output.add(double2String(d));
        }
        return output;
    }
    public static String double2Stringf(double d) {
        return String.format("%." + (DECIMAL_POINT_PRECISION) + "f", d);
    }
    public static ArrayList<String> double2Stringf(ArrayList<Double> list) {
        ArrayList<String> output = new ArrayList<String>(list.size());
        for (double d : list) {
            output.add(double2Stringf(d));
        }
        return output;
    }

    public static ArrayList<Integer> range(int start, int end) {
        if (end <= start) {
            return new ArrayList<Integer>();
        }
        ArrayList<Integer> output = new ArrayList<Integer>(end - start + 1);
        for (int i = start; i < end; i++) {  // exclude end
            output.add(i);
        }
        return output;
    }
    public static ArrayList<Double> rangeDouble(int start, int end) {
        if (end <= start) {
            return new ArrayList<Double>();
        }
        ArrayList<Double> output = new ArrayList<Double>(end - start + 1);
        for (double i = start; i < end; i++) {  // exclude end
            output.add(i);
        }
        return output;
    }

    public static double abs(double x) {
        return (x >= 0) ? x : -x;
    }

    public static double sum(ArrayList<Double> list) {
        double sum = 0d;
        for (int i = 0; i < list.size(); i++) {
            sum += list.get(i);
        }
        return sum;
    }

    public static double average(ArrayList<Double> list) {
        return sum(list) / list.size();
    }

    public static double power(double x, double y) {
        double output = x;
        for (int i = 1; i < y; i++) {
            output *= x;
        }
        return output;
    }

    public static double sqrt(double x) {
        if (x < 0) {
            throw new IllegalArgumentException("x(" + x + ") < 0");
        } else if (x == 0) {
            return 0d;
        } else if (x == 1) {
            return 1d;
        }
        double ouput = x;
        double upper = ((x <= 1) ? 1 : x);
        double lower = 0;
        double error = x;
        while (error > PRECISION) {
            ouput = (upper + lower) / 2;
            error = x - ouput * ouput;
            if (error > 0) {
                lower = ouput;
            } else {
                upper = ouput;
            }
            error = ListMath.abs(error);
        }
        return ouput;
    }

    public static double standardDeviation(ArrayList<Double> list) {
        double a = average(list);
        double deltaSquareSum = 0d;
        int n = list.size();
        for (int i = 0; i < n; i++) {
            double deltaX = list.get(i) - a;
            deltaSquareSum += (deltaX * deltaX);
        }
        return sqrt(deltaSquareSum / (n - 1));
    }

    public static ArrayList<Double> regressionLine(ArrayList<Double> listX, ArrayList<Double> listY) {
        if (listX.size() != listY.size()) {
            throw new IllegalArgumentException("listX.size != listY.size (" + listX.size() + ", " + listY.size() + ")");
        }
        ArrayList<Double> output = new ArrayList<Double>(2);  // (slope, y-intercept)
        double xAverage = average(listX);
        double yAverage = average(listY);

        // slope
        double Sxy = 0d;
        double Sxx = 0d;
        for (int i = 0; i < listX.size(); i++) {
            double deltaX = listX.get(i) - xAverage;
            Sxy += (deltaX * (listY.get(i) - yAverage));
            Sxx += (deltaX * deltaX);
        }

        double slope = Sxy / Sxx;
        output.add(slope);  // index = 0

        // y-intercept
        output.add(yAverage - slope * xAverage);  // index = 1

        System.out.println(slope);
        System.out.println(yAverage - slope * xAverage);

        return output;
    }
}


// read
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
    
    public Reader(String path) throws IOException {
        this(new File(path));
    }
    public Reader(File file) throws IOException {
        this.open(file);
    }
    
    private void open(File file) throws IOException {
        this.fileEmptyTF = (file.length() == 0);
        // open
        this.reader = new BufferedReader(new FileReader(file));
        this.readerOpenTF = true;
    }

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
    
    public void close() throws IOException {
        if (this.readerOpenTF == true) {
            this.reader.close();
            this.readerOpenTF = false;
            this.fileEmptyTF = false;
        }
    }
}

class CsvReader extends Reader {
    public static final String SEP = ",";

    public CsvReader(String path) throws IOException {
        super(path);
    }
    public CsvReader(File file) throws IOException {
        super(file);
    }

    public String[] readLineSplit() throws IOException {
        String output = super.readLine();
        if (output != null) {  // if there are datas in file
            return output.split(SEP);
        } else {  // if we have read all datas
            return null;
        }
    }
}


// write
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
    
    public Writer(String path, boolean appendTF) throws IOException {
        this(new File(path), appendTF);
    }
    public Writer(String path) throws IOException {
        this(new File(path));
    }
    public Writer(File file) throws IOException {
        this(file, false);
    }
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

class CsvWriter extends Writer {
    public static final String SEP = ",";
    public static final String END_LINE = "\n";
    public CsvWriter(String path) throws IOException {
        super(path);
    }
    public CsvWriter(String path, boolean appendTF) throws IOException {
        super(path, appendTF);
    }
    public CsvWriter(File file) throws IOException {
        super(file);
    }
    public CsvWriter(File file, boolean appendTF) throws IOException {
        super(file, appendTF);
    }

    public void write(String s, String end) throws IOException {
        super.write(s);
        super.write(end);
    }
    @Override public void write(String s) throws IOException {  // default: SEP
        super.write(s);
        super.write(SEP);
    }
    public void write(String[] array, String end) throws IOException {
        for (int i = 0; i < array.length; i++) {
            super.write(array[i]);
            if (i != array.length - 1) {  // not the last one
                super.write(SEP);
            }
        }
        super.write(end);
    }
    public void write(String[] array) throws IOException {  // default: END_LINE
        this.write(array, END_LINE);
    }
    public void write(ArrayList<String> list, String end) throws IOException {
        for (int i = 0; i < list.size(); i++) {
            super.write(list.get(i));
            if (i != list.size() - 1) {  // not the last one
                super.write(SEP);
            }
        }
        super.write(end);
    }
    public void write(ArrayList<String> list) throws IOException {  // default: END_LINE
        this.write(list, END_LINE);
    }
}
