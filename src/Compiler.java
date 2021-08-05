import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Compiler {

	private String sourceFile;

	// Function Identifiers
	private String constructorIdentifier = "constructor";
	private String importIdentifier = "import";
	private String classIdentifier = "class";
	private String voidIdentifier = "void";
	private String functionIdentifer = "function";
	private String returnIdentifier = "return";

	// C Identifiers
	// GOAL: Inherits the C stuff and makes it safer from there
	private String[] c_identifers = { "void", "char*", "FILE", "long" };
	private String[] c_functions = { "fopen", "fprintf", "malloc", "strcat" };

	// Import stuff
	private static final String IMPORT_BASICS = "#include <stdio.h>\n" + "#include <stdlib.h>\n"
			+ "#include <stdbool.h>\n";

	// Others
	private String headers = "";
	private boolean inConstructor = false;
	private int scopeStatus = 0; // 0 = class, 1 = function

	private List<HorseClass> objects;

	public Compiler(String sourceFile) {
		this.sourceFile = sourceFile;
		objects = new ArrayList<>();
	}

	// c_printf()
	public void start(String destFile) throws FileNotFoundException, IOException {
		try (BufferedReader r = new BufferedReader(new FileReader(sourceFile))) {
			String line;
			while ((line = r.readLine()) != null) {
				String cleanOne = cleanLine(line);

				System.out.println("Clean One: " + cleanOne);

				if (cleanOne.isEmpty() || cleanOne.startsWith("//"))
					continue;

				if (cleanOne.startsWith(constructorIdentifier)) {
					executeConstructor(cleanOne);

				} else if (cleanOne.startsWith(importIdentifier)) {
					executeImportIdentifier(cleanOne);

				} else if (cleanOne.startsWith(classIdentifier)) {
					executeClassIdentifier(cleanOne);

				} else if (cleanOne.startsWith("function")) {
					executeFunctionDeclaration(cleanOne);

				} else if (cleanOne.startsWith(returnIdentifier)) {
					executeReturnIdentifier(cleanOne);

				} else if (cleanOne.startsWith("if")) {
					executeIfFunction(cleanOne);

				} else if (cleanOne.startsWith("for")) {
					executeForFuction(cleanOne);

				} else if (cleanOne.startsWith("end")) {
					executeEndIdentifier();

				} else {
					boolean isSomething = false;
					for (String cfunc : c_functions) {
						if (cleanOne.startsWith(cfunc)) {
							executeCFunction(cleanOne);
							isSomething = true;
							break;
						}
					}

					for (String cIdent : c_identifers) {
						if (cleanOne.startsWith(cIdent)) {
							System.out.println("C Identifier: " + cIdent);
							executeIdentifer(cleanOne);
							isSomething = true;
							break;
						}
					}

					for (int i = 0; i < objects.size(); i++) {
						HorseClass current = objects.get(i);
						if (cleanOne.startsWith(current.getName())) {
							executeIdentifer(cleanOne);
							isSomething = true;
							break;
						}
					}

					if (isSomething)
						continue;

					// If nothing is found, then just use the cleanOne string with a semicolon
					HorseClass current = objects.get(objects.size() - 1);
					String classStuff = current.getName() + "->";
					if (inConstructor) {
						current.addConstructorLine(classStuff + cleanOne.replaceAll("\\.", "->") + ";");
					} else {
						current.add(classStuff + cleanOne.replaceAll("\\.", "->") + ";");
					}

				}
			}
		}

		for (HorseClass cls : objects) {
			try (FileWriter w = new FileWriter(Main.testOrNot + cls.getName() + ".c")) {
				w.append(cls.getPackage());
			}
		}
	}

	private void executeCFunction(String code) {
		// fprintf()
		String replaced = "";
		if (code.contains("."))
			replaced = code.replaceAll(".", "->");
		else
			replaced = code;
		objects.get(objects.size() - 1).add(replaced + ";");
	}

	private void executeIdentifer(String code) {
		// Example: char *name = malloc()
		String[] splitResult = splitSpecial(' ', code);
		if (splitResult.length <= 1)
			return;

		// so far char *name is guaranteed to be present
		HorseClass current = objects.get(objects.size() - 1);

		if (splitResult.length == 3) {
			// char *name = malloc() or char *name = previousName
			if (indexOfSpecial('(', splitResult[1]) != -1) {
				// substring(1) for accounting for the extra space in between
				executeCFunction(splitResult[1].substring(1));
			} else {
				String identifierWithoutPointer = splitResult[1].contains("*") ? splitResult[1].substring(1)
						: splitResult[1];
				current.add(identifierWithoutPointer + " = " + splitResult[1] + ";");
			}
		} else if (splitResult.length == 5) {
			// IO i = new IO()
			current.add(splitResult[0] + "*" + splitResult[1] + "=" + splitResult[4] + ";");
			return;
		}
		current.addVariable(splitResult[0] + ";");
		current.addVariableIndex(splitResult[1]);
	}

	// Class/Object Functions
	private void executeClassIdentifier(String code) {
		// Example: class Name extends Object
		String[] splitResult = code.split(" ");
		if (splitResult.length >= 2) {
			// class Name
			objects.add(new HorseClass(splitResult[1], headers));

			if (splitResult.length == 4) {
				// class Name extends Object
				HorseClass current = objects.get(objects.size() - 1);
				current.addVariable(splitResult[3] + " *__extends;");
				current.addVariableIndex(splitResult[1]);

				// Add __extends to the constructor
				current.addConstructorLine("__extends = " + splitResult[3] + "_constructor();");
			}
		}
		// Reset all necessary indicators
		headers = "";
		scopeStatus = 0;
	}

	private void executeConstructor(String code) {
		// constructor(char *arg)
		String className = objects.get(objects.size() - 1).getName();
		String argsWithParentheses = code.substring(constructorIdentifier.length());

		HorseClass current = objects.get(objects.size() - 1);
		current.addConstructorLineAt(0,
				className + "_t* " + className.concat("_").concat(constructorIdentifier) + argsWithParentheses + "{");
		current.addConstructorLine(
				className + "_t* " + "__" + className + "__obj = " + "malloc(sizeof(" + className + "_t));");

		inConstructor = true;
		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private boolean isCIdentifier(String code) {
		for (String cIdent : c_identifers)
			if (code.startsWith(cIdent))
				return true;
		return false;
	}

	private void executeFunctionDeclaration(String code) {
		// function sayHello() returns String
		String splitResult[] = code.split("returns");
		String functionHeader = splitResult[0].substring(functionIdentifer.length() + 1);
		String returnType = splitResult[1].substring(1);

		if (!isCIdentifier(returnType)) {
			returnType += "*";
		}
		// Add the function to the typedef struct & define it in the constructor
		HorseClass current = objects.get(objects.size() - 1);
		String functionName = functionHeader.substring(0, functionHeader.indexOf('('));
		String className = current.getName() + "_";
		if (functionName.contentEquals("main")) {
			className = "";
		}
		current.addVariable(
				returnType + "(*" + functionName + ") " + functionHeader.substring(functionHeader.indexOf('(')) + ";");
		current.addConstructorLine(
				"__" + current.getName() + "__obj->" + functionName + "=&" + className + functionName + ";");
		current.add(returnType + " " + className + functionHeader + "{");

		// Change the current parentheses scope status to 1
		scopeStatus++;
	}

	private void executeIfFunction(String code) {
		// if 5 == 5 then
		String ifheader = "if (" + code.substring("if".length() + 2, code.lastIndexOf("then")).replaceAll(".", "->")
				+ ") {";
		objects.get(objects.size() - 1).add(ifheader);

		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private void executeForFuction(String code) {
		// for int i = 0, i < 5, i++
		String forheader = "for (" + code.substring("for".length() + 2).replaceAll(",", ";") + ") {";
		objects.get(objects.size() - 1).add(forheader);

		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private void executeImportIdentifier(String code) {
		// Example: import basics
		String thingToImport = splitSpecial(' ', code)[1];
		if (thingToImport.contentEquals("basics")) {
			headers += IMPORT_BASICS;
		} else {
			headers += "#include <" + thingToImport + ".h>";
		}
	}

	private void executeReturnIdentifier(String code) {
		// Example: return 5
		String[] splitResult = splitSpecial(' ', code);
		objects.get(objects.size() - 1).add(returnIdentifier + " " + splitResult[1] + ";");
	}

	private void executeEndIdentifier() {
		HorseClass current = objects.get(objects.size() - 1);
		// Example: end
		if (inConstructor) {
			current.add("return __" + current.getName() + "__obj;");
			inConstructor = false;
		}

		if (scopeStatus >= 1)
			current.add("}");

		if (scopeStatus - 1 >= 0)
			scopeStatus--;
	}

	///////// Very important functions here /////////

	private String[] splitSpecial(char find, String str) {
		ArrayList<String> list = new ArrayList<>();
		boolean isQuote = false;
		int previousIndex = 0;
		char previousChar;
		if (str.length() > 0)
			previousChar = str.charAt(0);
		else
			previousChar = ' ';

		for (int i = 0; i < str.length(); i++) {
			char letter = str.charAt(i);
			if (letter == '"' || letter == '\'') {
				isQuote = !isQuote;
			} else if (!isQuote && letter == find && previousChar != find) {
				list.add(str.substring(previousIndex, i));
				previousIndex = i;
			}
			previousChar = letter;

		}
		int addNum = 1;
		if (previousIndex == 0 && str.charAt(previousIndex) != find)
			addNum = 0;
		list.add(str.substring(previousIndex + addNum, str.length()));

		return listToStringArray(list);
	}

	private int indexOfSpecial(char find, String str) {
		boolean isQuote = false;
		for (int i = 0; i < str.length(); i++) {
			char letter = str.charAt(i);
			if (letter == '"' || letter == '\'') {
				isQuote = !isQuote;
			} else if (!isQuote && letter == find) {
				return i;
			}
		}
		return -1;
	}

	private String[] listToStringArray(List<String> arr) {
		String[] strArray = new String[arr.size()];
		for (int i = 0; i < strArray.length; i++) {
			strArray[i] = arr.get(i);
		}
		return strArray;
	}

	/**
	 * Cleans the line of any tabs if it is not in a quote
	 * 
	 * @param originalLine
	 * @return
	 */
	private String cleanLine(String originalLine) {
		String newStr = "";
		boolean inQuote = false;
		boolean firstLetterReached = false;
		for (int i = 0; i < originalLine.length(); i++) {
			char letter = originalLine.charAt(i);
			// Removes spaces and tabs until the first character
			if (letter != ' ' && letter != '\t') {
				firstLetterReached = true;
			} else if (!firstLetterReached) {
				continue;
			}

			if (letter == '"' || letter == '\'') {
				inQuote = !inQuote;
				newStr += letter;
			} else if (!inQuote && letter != '\t') {
				newStr += letter;

			} else if (inQuote) {
				newStr += letter;
			}
		}
		return newStr;
	}
}