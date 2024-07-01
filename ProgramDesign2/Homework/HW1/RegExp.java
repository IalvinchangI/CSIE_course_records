import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class RegExp {
    private static String print_sep = ",";
    
    public static void main(String[] args) {
        String str1 = args[1];
        String str2 = args[2];
        int s2Count = Integer.parseInt(args[3]);  // str2 exceeds or equals n times

        try {
            BufferedReader reader = new BufferedReader(new FileReader(args[0]));
            String line;
            while ((line = reader.readLine()) != null) {
                ReString s = new ReString(line);
                System.out.println(
                    bool2word(s.palindrome_TF()) + print_sep + 
                    bool2word(s.contain_TF(str1)) + print_sep + 
                    bool2word(s.contain_times_TF(str2, s2Count)) + print_sep + 
                    bool2word(s.contain_AmXB2m_TF("a", "b"))
                );
                /* 
                System.out.println(line);
                System.out.print(bool2word(s.palindrome_TF()) + print_sep);
                System.out.print(bool2word(s.contain_TF(str1)) + print_sep);
                System.out.print(bool2word(s.contain_times_TF(str2, s2Count)) + print_sep);
                System.out.print(bool2word(s.contain_AmXB2m_TF("a", "b")) + "\n");
                */
            }
            reader.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static char bool2word(boolean TF) {
        return TF ? 'Y' : 'N';
    }
}

class ReString {
    // RegExp string
    private String data;

    /////////////////////////////////////////////////////////////////////
    public boolean palindrome_TF() {
        for (int startIndex = 0, endIndex = data.length() - 1; startIndex < endIndex; startIndex++, endIndex--) {
            if (data.charAt(startIndex) != data.charAt(endIndex)) {
                return false;
            }
        }
        return true;
    }

    public boolean contain_TF(String target) {
        return containAt(target, 0, "normal") != -1;
    }

    public boolean contain_times_TF(String target, int times) {
        int real_times = 0;
        int index = 0;
        while (true) {
            index = containAt(target, index, "normal");
            if (index == -1) {  // can not find
                break;
            }
            real_times++;
            index += target.length();
        }
        return real_times >= times;
    }

    public boolean contain_AmXB2m_TF(String a, String b) {
        int index_a = containAt(a, 0, "normal");  // there are "a" at the left hand part
        if (index_a == -1) {
            return false;
        }

        int index_2b = containAt(b + b, data.length(), "inverse");  // there are "bb" at the right hand part
        // int index_2b = -1;
        // {
        // int times = 0;
        // int index = data.length();
        // while (true) {  // find where the 2b are
        //     int new_index = containAt(b, index, "inverse");
        //     if (new_index == -1) {  // there is not any b
        //         break;
        //     }
        //     if (index - new_index == b.length()) {  // it is continuous b
        //         times++;
        //     } else {  // find 1 b
        //         times = 1;
        //     }
        //     if (times == 2) {  // find 2b
        //         index_2b = new_index;
        //         break;
        //     }
        //     index = new_index;
        // }
        // }
        return index_a < index_2b;  // index_2b != -1 if index_a < index_2b
    }
    /////////////////////////////////////////////////////////////////////

    public ReString(String data) {
        this.data = data;
        toLowerCase();
    }

    private void toLowerCase() {
        data = data.toLowerCase();
    }

    private int containAt(String target, int startIndex, String order) {
        /*
        return index if exist else -1
        order = "normal"  -> check str from left(low index) to right(high index)  &  contain startIndex
        order = "inverse" -> check str from right(high index) to left(low index)  &  not contain startIndex
        */
        int index = -1;
        if (order == "normal") {  // ->
            for (int i = startIndex; i <= data.length() - target.length(); i++) {
                if (checkChars(target, i)) {
                    index = i;
                    break;
                }
            }
        } else if (order == "inverse") {  // <-
            for (int i = startIndex - target.length(); i >= 0; i--) {
                if (checkChars(target, i)) {
                    index = i;
                    break;
                }
            }
        } else {  // error
            System.out.println("error: not exist the order of \"" + order + "\"");
        }
        return index;
    }

    private boolean checkChars(String target, int checkAt) {
        if (checkAt + target.length() > data.length()) {  // out of range
            return false;
        }
        for (int i = 0; i < target.length(); i++) {
            if (target.charAt(i) != data.charAt(checkAt + i)) {
                return false;
            }
        }
        return true;
    }
}