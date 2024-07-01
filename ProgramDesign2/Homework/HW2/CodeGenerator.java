import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;

import java.util.regex.Pattern;
import java.util.regex.Matcher;




public class CodeGenerator {
    public static void main(String[] args) {
        try {
            ClassDiagramReader reader = new ClassDiagramReader(args[0]);
            reader.split();
            reader.close();

            ClassDiagram compiler = new ClassDiagram(reader.getContent());
            compiler.compile();
            /*
            compiler.printCompileResult();
            /*/
            for (String className : compiler.getClassNames()) {
                try {
                    Writer writer = new Writer(className + ".java");
                    writer.write(compiler.getClassManager(className).toString());
                    writer.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            //*/
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}



class WordReader {
    protected BufferedReader reader = null;
    protected boolean readerOpenTF = false;
    public boolean hasOpen() {
        return this.readerOpenTF;
    }
    protected ArrayList<ArrayList<String>> content = null;
    public ArrayList<ArrayList<String>> getContent() {
        return this.content;
    }
    
    public WordReader(String path) throws IOException {
        // open
        this.reader = new BufferedReader(new FileReader(path));
        this.readerOpenTF = true;
    }
    
    
    public ArrayList<ArrayList<String>> split() throws IOException {
        ArrayList<ArrayList<String>> output = new ArrayList<ArrayList<String>>();
        String line = null;
        while ((line = this.reader.readLine()) != null) {  // read file and put substring into ArrayList
            ArrayList<String> content = split(line);
            if (content.size() != 0) {  // nonempty
                output.add(content);
            }
        }
        this.content = output;
        return this.content;
    }
    
    protected ArrayList<String> split(String s) {
        return new ArrayList<String>(Arrays.asList(s.split("\\s+")));
    }
    
    public void printContent() {
        if (this.content != null) {
            for (ArrayList<String> array : this.content) {
                System.out.println(array);
            }
        } else {
            System.out.println("Have not split");;
        }
    }

    public void openNew(String path) throws IOException {
        if (this.readerOpenTF == false) {
            // open
            this.reader = new BufferedReader(new FileReader(path));
            this.readerOpenTF = true;
            // reset
            this.content.clear();
            this.content = null;
        }
    }
    public void close() throws IOException {
        if (this.readerOpenTF == true) {
            this.reader.close();
            this.readerOpenTF = false;
        }
    }
}
class ClassDiagramReader extends WordReader {
    private static final Pattern SPLIT_TARGET_PATTERN = Pattern.compile("[\\(\\)\\[\\]\\{\\}\\<\\>\\:\\+\\-\\,]");  // target "(", ")", "[", "]", "{", "}", ":", "+", "-", ","
    
    public ClassDiagramReader(String path) throws IOException {
        super(path);
    }
    
    @Override
    protected ArrayList<String> split(String s) {
        ArrayList<String> output = new ArrayList<String>();
        
        for (String word : super.split(s)) {
            Matcher matcher = SPLIT_TARGET_PATTERN.matcher(word);
            int startSplit = 0;
            while (matcher.find()) {
                int endSplit = matcher.start();
                if (endSplit > startSplit) {  // split the word before target
                    output.add(word.substring(startSplit, endSplit));
                }
                startSplit = matcher.end();
                output.add(word.substring(endSplit, startSplit));  // split target
            }
            if (startSplit < word.length()) {  // split the last word
                output.add(word.substring(startSplit));
            }
        }
        return output;
    }
}


class ClassDiagram {
    // class
    private static final String CLASS_KEYWORD = "class";
    private static final String IDENTIFIER_KEYWORD = ":";
    private static final String CLASS_BLOCK_START_KEYWORD = "{";
    private static final String CLASS_BLOCK_END_KEYWORD = "}";
    
    // object
    private ArrayList<ArrayList<String>> content = null;
    private ArrayList<String> classNames = null;  // class writing order
    public ArrayList<String> getClassNames() {
        return this.classNames;
    }
    private HashMap<String, ClassManager> class2ClassManager = null;  // store ClassManager object
    public ClassManager getClassManager(String name) {
        return this.class2ClassManager.get(name);
    }

    public ClassDiagram(ArrayList<ArrayList<String>> content) {
        this.content = new ArrayList<ArrayList<String>>();
        this.classNames = new ArrayList<String>();
        this.class2ClassManager = new HashMap<String, ClassManager>();

        load(content);
    }
    
    private void load(ArrayList<ArrayList<String>> content) {  // load content
        ArrayList<String> classDiagram = new ArrayList<String>(1);
        classDiagram.add("classDiagram");

        int start = content.indexOf(classDiagram);
        if (start != -1) {
            this.content.addAll(content.subList(start + 1, content.size()));
        } else {
            throw new IllegalArgumentException("Not classDiagram");
        }
    }
    
    public void compile() {
        String currentClass = null;
        boolean inBlock = false;
        for (ArrayList<String> line : this.content) {
            if (currentClass == null) {
                if (line.get(0).equals(CLASS_KEYWORD)) {  // >>> class className ...
                    String className = line.get(1);
                    if (this.classNames.contains(className) == false) {  // add new class name
                        this.classNames.add(className);
                        this.class2ClassManager.put(className, new ClassManager(className));
                    }
                    if (line.size() > 2 && line.get(2).equals(CLASS_BLOCK_START_KEYWORD)) {  // >>> class className { ...
                        currentClass = className;
                        inBlock = true;
                        line = new ArrayList<String>(line.subList(3, line.size()));
                    } else {  // >>> class className
                        continue;
                    }
                } else if (this.classNames.contains(line.get(0)) && line.get(1).equals(IDENTIFIER_KEYWORD)) {  // >>> className : ...
                    currentClass = line.get(0);
                    line = new ArrayList<String>(line.subList(2, line.size()));
                }
                if (currentClass == null) {
                    throw new IllegalArgumentException("Not classDiagram syntax");
                }
            }
            if (line.contains(CLASS_BLOCK_END_KEYWORD)) {  // >>> ... }
                if (line.indexOf(CLASS_BLOCK_END_KEYWORD) + 1 < line.size()) {  // >>> ... } ...
                    throw new IllegalArgumentException("Not classDiagram syntax");
                }
                line = new ArrayList<String>(line.subList(0, line.size() - 1));
                inBlock = false;
            }
            if (line.size() != 0) {
                class2ClassManager.get(currentClass).addIdentifier(line);
            }
            if (inBlock == false) {
                currentClass = null;
                continue;
            }
        }
    }
    
    public void printCompileResult() {
        for (String className : this.classNames) {
            System.out.println(class2ClassManager.get(className));
        }
    }
}

abstract class StantardOutputWord {
    protected static final String NEW_LINE = "\n";
    protected static final String SEP = " ";
    protected static final String TAB = "    ";
    protected static final String LEFT_SHARP_BRACKET = "<";
    protected static final String RIGHT_SHARP_BRACKET = ">";
    protected static final String LEFT_BRACE = "{";
    protected static final String RIGHT_BRACE = "}";
    protected static final String LEFT_SQUARE_BRACKET = "[";
    protected static final String RIGHT_SQUARE_BRACKET = "]";
    protected static final String LEFT_ROUND_BRACKET = "(";
    protected static final String RIGHT_ROUND_BRACKET = ")";
    protected static final String END_LINE = ";";
    protected static final String COMMA = ",";
    protected static final String RETURN = "return";
    protected static final String THIS = "this.";
    protected static final String ASSIGN = "=";

    protected static boolean isLeftBracketTF(String target) {
        return target.equals(LEFT_SHARP_BRACKET) ||
               target.equals(LEFT_BRACE) ||
               target.equals(LEFT_SQUARE_BRACKET) ||
               target.equals(LEFT_ROUND_BRACKET);
    }
    protected static boolean isRightBracketTF(String target) {
        return target.equals(RIGHT_SHARP_BRACKET) ||
               target.equals(RIGHT_BRACE) ||
               target.equals(RIGHT_SQUARE_BRACKET) ||
               target.equals(RIGHT_ROUND_BRACKET);
    }
    protected static boolean isBracketTF(String target) {
        return isLeftBracketTF(target) || isRightBracketTF(target);
    }
    protected static boolean isCommaTF(String target) {
        return target.equals(COMMA);
    }
}

class ClassManager extends StantardOutputWord {
    protected String name = null;
    protected ArrayList<Identifier> identifiers = null;  // store the order of identifier

    protected static final String CLASS = "public class";

    public ClassManager(String name) {
        this.name = name;
        this.identifiers = new ArrayList<Identifier>();
    }

    public void addIdentifier(ArrayList<String> tokens) {
        if (tokens.contains("(")) {  // is Method
            this.identifiers.add(new Method(tokens));
        } else {
            this.identifiers.add(new Attribute(tokens));
        }
    }

    public String toString() {
        String buffer = CLASS + SEP + this.name + SEP + LEFT_BRACE + NEW_LINE;
        for (Identifier identifier : identifiers) {
            buffer += (TAB + identifier.toString().replaceAll(NEW_LINE, NEW_LINE + TAB) + NEW_LINE);
        }
        buffer += RIGHT_BRACE;
        return buffer;
    }
}
                        
abstract class Identifier extends StantardOutputWord {
    protected String identifier = null;  // attribute | method(args' type); <<for equals>>
    protected String modifier = null;
    protected String type = null;
    protected String name = null;  // attribute | method(args' type & name)
    
    protected ArrayList<String> tokens = null;
    
    protected static final HashMap<String, Integer> SQUARE_BRACKET_2_LAYER_UPDOWN = initBracketMap(LEFT_SQUARE_BRACKET, RIGHT_SQUARE_BRACKET);  // []
    protected static final HashMap<String, Integer> SHARP_BRACKET_2_LAYER_UPDOWN = initBracketMap(LEFT_SHARP_BRACKET, RIGHT_SHARP_BRACKET);  // <>
    protected static HashMap<String, Integer> initBracketMap(String up, String down) {
        HashMap<String, Integer> output = new HashMap<String, Integer>();
        output.put(up, 1);
        output.put(down, -1);
        return output;
    }
    
    
    public Identifier(ArrayList<String> tokens) {
        this.tokens = tokens;
        setModifier(tokens.get(0));
    }
    
    protected void setModifier(String token) throws IllegalArgumentException {
        switch (token) {
            case "+":
                this.modifier = "public";
                break;
            case "-":
                this.modifier = "private";
                break;
            default:
                throw new IllegalArgumentException("Modifier Not Found");
        }
    }
    
    protected void setType(String type) {
        this.type = type;
    }
    
    protected int setType(int tokenStart) throws IllegalArgumentException {
        String buffer = this.tokens.get(tokenStart);
        int sharpBracketLayer = 0;
        int squareBracketLayer = 0;
        
        int endIndex;
        for (endIndex = tokenStart + 1; endIndex < this.tokens.size(); endIndex++) {
            if (SHARP_BRACKET_2_LAYER_UPDOWN.keySet().contains(this.tokens.get(endIndex))) {  // tokens[endIndex] == "<" | ">"
                sharpBracketLayer += SHARP_BRACKET_2_LAYER_UPDOWN.get(this.tokens.get(endIndex));
            } else if (SQUARE_BRACKET_2_LAYER_UPDOWN.keySet().contains(this.tokens.get(endIndex))) {  // tokens[endIndex] == "[" | "]"
                squareBracketLayer += SQUARE_BRACKET_2_LAYER_UPDOWN.get(this.tokens.get(endIndex));
            } else if (sharpBracketLayer > 0 || squareBracketLayer > 0) {
                buffer += this.tokens.get(endIndex);  // += "..."
                if (isBracketTF(this.tokens.get(endIndex + 1)) == false && 
                    isCommaTF(this.tokens.get(endIndex + 1)) == false && 
                    isLeftBracketTF(this.tokens.get(endIndex)) == false) {  // "...<abc" | "abc>" | "...[abc" | "abc]" | "abc,"
                    buffer += SEP;
                }
                continue;
            } else {
                break;
            }
            buffer += this.tokens.get(endIndex);  // += [] | <>
        }
        if (squareBracketLayer != 0) {
            throw new IllegalArgumentException("Syntax error");
        }
        this.type = buffer;
        return endIndex;
    }
    
    @Override
    public boolean equals(Object other) {
        if (other == null) return false;
        if (this.getClass() != other.getClass()) return false;
        
        Identifier realOther = (Identifier) other;
        return this.identifier.equals(realOther.identifier);
    }

    @Override
    public String toString() {
        return this.modifier + SEP + this.type + SEP + this.name;
    }
}

class Attribute extends Identifier {
    public Attribute(ArrayList<String> tokens) {
        super(tokens);
        int nameStart = setType(1);
        setName(nameStart);
        this.identifier = this.name;
    }
    
    protected void setName(int tokensStart) {
        this.name = this.tokens.get(tokensStart);
    }
    
    @Override
    public String toString() {
        return super.toString() + END_LINE;
    }
}

class Method extends Identifier {
    protected static final HashMap<String, Integer> ROUND_BRACKET_2_LAYER_UPDOWN = initBracketMap(LEFT_ROUND_BRACKET, RIGHT_ROUND_BRACKET);
    
    protected ArrayList<String> argumentTypes = null;
    protected ArrayList<String> argumentNames = null;
    protected String statement = null;

    private static HashMap<String, String> returnDefaultValue = initReturnDefaultValue();
    private static HashMap<String, String> initReturnDefaultValue() {
        HashMap<String, String> output = new HashMap<String, String>();
        output.put("int", "0");
        output.put("String", "\"\"");
        output.put("boolean", "false");
        return output;
    }
    
    protected static Pattern propertyPattern = Pattern.compile("(get|set)([A-Z]\\w*)\\(");  // (get|set) (method name)
    protected static Pattern argumentPattern = Pattern.compile("([\\w\\[\\]\\<\\>]+)\\s(\\w+)");  // (type) (argument name)
    protected static Pattern argumentsPattern = Pattern.compile("(\\w+)\\((.*)\\)");  // (name) (arguments)
    
    public Method(ArrayList<String> tokens) {
        super(tokens);
        int typeStart = setName(1);
        if (typeStart >= tokens.size()) {  // if type is empty
            setType("void");
        } else {
            setType(typeStart);
        }
        setArguments();
        setIdentifier();
        setStatement();
    }
    
    protected int setName(int tokenStart) throws IllegalArgumentException {
        String buffer = this.tokens.get(tokenStart);
        int roundBracketLayer = 0;
        
        int endIndex;
        for (endIndex = tokenStart + 1; endIndex < this.tokens.size(); endIndex++) {
            if (ROUND_BRACKET_2_LAYER_UPDOWN.keySet().contains(this.tokens.get(endIndex))) {  // tokens[endIndex] == "(" | ")"
                roundBracketLayer += ROUND_BRACKET_2_LAYER_UPDOWN.get(this.tokens.get(endIndex));
            } else if (roundBracketLayer > 0) {
                buffer += this.tokens.get(endIndex);  // += "..."
                if (isBracketTF(this.tokens.get(endIndex + 1)) == false && 
                    isCommaTF(this.tokens.get(endIndex + 1)) == false && 
                    isLeftBracketTF(this.tokens.get(endIndex)) == false) {  // "...(abc" | "abc)" | "abc,"
                    buffer += SEP;
                }
                continue;
            } else {
                break;
            }
            buffer += this.tokens.get(endIndex);  // += ()
        }
        if (roundBracketLayer != 0) {
            throw new IllegalArgumentException("Syntax error");
        }
        this.name = buffer;
        return endIndex;
    }

    protected void setArguments() {
        Matcher argsMatcher = argumentsPattern.matcher(this.name);
        this.argumentTypes = new ArrayList<String>();
        this.argumentNames = new ArrayList<String>();
        if (argsMatcher.find()) {
            Matcher argumentMatcher = argumentPattern.matcher(argsMatcher.group(2));
            while (argumentMatcher.find()) {
                this.argumentTypes.add(argumentMatcher.group(1));
                this.argumentNames.add(argumentMatcher.group(2));
            }
        } else {
            throw new IllegalArgumentException("Method not found");
        }
    }
    
    protected void setIdentifier() {
        Matcher indentifierMatcher = argumentsPattern.matcher(this.name);
        if (indentifierMatcher.find()) {
            this.identifier = indentifierMatcher.group(1) + LEFT_ROUND_BRACKET;
            if (this.argumentTypes.size() > 0) {
                int i = 0;
                while (true) {
                    this.identifier += this.argumentTypes.get(i);
                    i++;
                    if (i >= this.argumentTypes.size()) {  // end loop
                        break;
                    }
                    this.identifier += (COMMA + SEP);
                }
            }
            this.identifier += RIGHT_ROUND_BRACKET;
        } else {
            throw new IllegalArgumentException("Method not found");
        }
    }
    
    protected void setStatement() {
        String buffer = LEFT_BRACE;
        Matcher propertyMatcher = propertyPattern.matcher(this.name);
        if (propertyMatcher.find()) {  // setter or getter
            try {
                String tempBuffer = NEW_LINE;
                switch (propertyMatcher.group(1)) {
                    case "get":
                        tempBuffer += getterContent(lowerCaseAt(propertyMatcher.group(2), 0));
                    break;
                    case "set":
                        tempBuffer += setterContent(lowerCaseAt(propertyMatcher.group(2), 0));
                    break;
                }
                buffer += (tempBuffer + NEW_LINE);
            } catch (IllegalArgumentException e) {
                if (returnDefaultValue.keySet().contains(this.type)) {
                    buffer += (RETURN + SEP + returnDefaultValue.get(this.type));
                }
                buffer += END_LINE;
            }
        } else {  // normal
            if (returnDefaultValue.keySet().contains(this.type)) {
                buffer += (RETURN + SEP + returnDefaultValue.get(this.type));
            }
            buffer += END_LINE;
        }
        buffer += RIGHT_BRACE;
        this.statement = buffer;
    }

    protected String setterContent(String attribute) {
        if (this.argumentNames.size() != 1) {
            throw new IllegalArgumentException("Not setter");
        }
        return TAB + THIS + attribute + SEP + ASSIGN + SEP + this.argumentNames.get(0) + END_LINE;
    }
    protected String getterContent(String attribute) {
        return TAB + RETURN + SEP + attribute + END_LINE;
    }

    protected String lowerCaseAt(String s, int at) {
        return s.substring(0, at) + Character.toLowerCase(s.charAt(at)) + s.substring(at + 1);
    }

    @Override
    public String toString() {
        return super.toString() + SEP + this.statement;
    }
}


class Writer {
    protected BufferedWriter writer = null;
    protected boolean writerOpenTF = false;
    public boolean hasOpen() {
        return this.writerOpenTF;
    }
    
    public Writer(String path) throws IOException {
        File file = new File(path);
        if (!file.exists()) {
            file.createNewFile();
        }
        // open
        this.writer = new BufferedWriter(new FileWriter(file));
        this.writerOpenTF = true;
    }
    
    public void write(String s) throws IOException {
        this.writer.write(s);
    }
    
    public void openNew(String path) throws IOException {
        if (this.writerOpenTF == false) {
            File file = new File(path);
            if (!file.exists()) {
                file.createNewFile();
            }
            // open
            this.writer = new BufferedWriter(new FileWriter(file));
            this.writerOpenTF = true;
        }
    }
    public void close() throws IOException {
        if (this.writerOpenTF == true) {
            this.writer.close();
            this.writerOpenTF = false;
        }
    }
}
