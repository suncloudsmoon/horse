/*
 * Copyright (c) 2021, suncloudsmoon and the horse contributors.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
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
	private String[] c_identifers = { "void", "char", "long", "FILE" };
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

	// "All roads lead to Rome"... is here
	public void start(String destFile) throws FileNotFoundException, IOException {
		try (BufferedReader r = new BufferedReader(new FileReader(sourceFile))) {
			String line;
			while ((line = r.readLine()) != null) {
				String cleanOne = cleanLine(line);

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
					for (String cIdent : c_identifers) {
						if (cleanOne.startsWith(cIdent)) {
							executeIdentifer(cleanOne);
							isSomething = true;
							break;
						}
					}
					// For identifying instructions that create objects: IO i = new IO()
					for (int i = 0; i < objects.size(); i++) {
						HorseClass current = objects.get(i);
						if (cleanOne.startsWith(current.getName())) {
							executeIdentifer(cleanOne);
							isSomething = true;
							break;
						}
					}
					// For processing existing variables: text = malloc()
					HorseClass current = objects.get(objects.size() - 1);
					for (String var : objects.get(objects.size() - 1).getVariableIndexList()) {
						if (cleanOne.startsWith(var)) {
							// text = malloc()
							String[] splitResult = splitSpecial(' ', cleanOne);
							String compiledLine = "__" + current.getName() + "__obj->" + splitResult[0];
							// If cleanOne has a function here: text = malloc()
							if (indexOfSpecial('(', cleanOne) != -1) {
								compiledLine += "=" + dealWithFunctionCalls(splitResult[2]);
							} else {
								compiledLine += splitResult[1].concat(" ").concat(splitResult[2]) + ";";
							}
							if (inConstructor) {
								current.addConstructorLine(compiledLine);
							} else {
								current.add(compiledLine);
							}
							isSomething = true;
							break;
						}
					}
					if (isSomething)
						continue;

					// If nothing is found, then just use the cleanOne string with a semicolon
					generalBehavior(cleanOne);
				}
			}
		}

		// Export the compiled C code in String to File
		for (HorseClass cls : objects) {
			cls.writeToFile(Main.testOrNot);
		}
	}

	private void generalBehavior(String code) {
		HorseClass current = objects.get(objects.size() - 1);
		String addStr = "";

		if (indexOfSpecial('(', code) != -1) {
			dealWithFunctionCalls(code);
		} else if (isCIdentifier(code)) {
			executeIdentifer(code);
		} else {
			addStr = code + ";";
		}
		if (inConstructor)
			current.addConstructorLine(addStr);
		else
			current.add(addStr);
	}

	// TODO: implement try catch system
	// TODO: do some documentation now

	private String dealWithFunctionCalls(String code) {
		// io.print()
		HorseClass current = objects.get(objects.size() - 1);
		int dotIndex = indexOfSpecial('.', code);
		String formattedLine = "";

		if (dotIndex == -1) {
			// print()
			// Or the function call must be made inside a function
			// TODO: fix issues when dealing with objects in the main method (or not?) - it
			// is a feature not a bug?
			if (!isCFunction(code)) {
				System.out.println("Code: " + code);
				String functionName = code.substring(0, code.indexOf('(') + 1);
				String restOfParameters = code.substring(code.indexOf('(') + 1);
				formattedLine += executeFunction(functionName + "__" + current.getName() + "__obj," + restOfParameters);
			} else {
				formattedLine += code + ";";
			}

		} else {
			// input.print()
			String[] dotData = splitSpecial('.', code);
			formattedLine += dotData[0] + "->";
			for (int i = 1; i < dotData.length; i++) {
				String function = dotData[i];
				String functionName = function.substring(0, function.indexOf('(') + 1);
				String restOfParameters = function.substring(function.indexOf('(') + 1);
				formattedLine += executeFunction(functionName + "(" + dotData[0] + "," + restOfParameters);
				if (i != dotData.length - 1)
					formattedLine += "->";
			}
			formattedLine += ";";
		}
		return formattedLine;
	}

	private boolean isCFunction(String code) {
		for (String str : c_functions)
			if (code.startsWith(str))
				return true;
		return false;
	}

	// TODO: create a special case use of main() function
	private String executeFunction(String code) {
		// fprintf()
		int paranthesisIndex = code.indexOf('(');

		String functionName = code.substring(0, paranthesisIndex);
		String parameters = code.substring(paranthesisIndex + 1, code.lastIndexOf(')'));
		String[] parameterList = splitSpecial(',', parameters);

		String fullFunctionDetails = functionName + "("
				+ (parameterList.length > 1 ? parameterList[0].substring(1) + "," : parameterList[0].substring(1));

		HorseClass current = objects.get(objects.size() - 1);
		List<String> varList = current.getVariableIndexList();

		for (int i = 1; i < parameterList.length; i++) {
			String var = parameterList[i];
			if (isVariable(var))
				fullFunctionDetails += "__" + objects.get(objects.size() - 1).getName() + "__obj->" + var;
			else
				fullFunctionDetails += var;
			if (i != parameterList.length - 1)
				fullFunctionDetails += ",";
		}
		fullFunctionDetails += ");";

		return fullFunctionDetails;
	}

	private boolean isVariable(String variable) {
		for (String var : objects.get(objects.size() - 1).getVariableIndexList())
			if (variable.contentEquals(var))
				return true;
		return false;
	}

	private void executeIdentifer(String code) {
		// Example: char *name = malloc()
		String[] splitResult = splitSpecial(' ', code);
		if (splitResult.length <= 1)
			return;

		// so far char *name is guaranteed to be present
		HorseClass current = objects.get(objects.size() - 1);

		String pointerOrNot = " ";
		String objectAccess = "__" + current.getName() + "__obj->";
		if (splitResult.length == 4) {
			// char *name = malloc() or char *name = previousName
			String identifierWithoutPointer = splitResult[1].contains("*") ? splitResult[1].substring(1)
					: splitResult[1];

			String varDeclaration = objectAccess + identifierWithoutPointer + " = ";
			if (indexOfSpecial('(', splitResult[3]) != -1) {
				current.add(varDeclaration + dealWithFunctionCalls(splitResult[3]) + ";");
			} else {
				current.add(varDeclaration + splitResult[3] + ";");
			}
			return;
		} else if (splitResult.length == 5) {
			// IO i = new IO()
			current.add(splitResult[1] + "=" + splitResult[4].substring(0, splitResult[4].indexOf('(')) + "_constructor"
					+ splitResult[4].substring(splitResult[4].indexOf('(')) + ";");
			// Custom variable addition
			pointerOrNot = "*";
			return;
		}
		current.addVariable(splitResult[0] + pointerOrNot + splitResult[1] + ";");
		current.addVariableIndex(splitResult[1]);
	}

	// Class/Object Functions
	private void executeClassIdentifier(String code) {
		// Example: class Name
		String[] splitResult = code.split(" ");
		HorseClass newObj = new HorseClass(splitResult[1], headers);
		objects.add(newObj);
		if (splitResult.length == 4) {
			// class Name extends Object
			newObj.addVariable(splitResult[3] + "_t *__extends;");
			newObj.addVariableIndex(splitResult[1]);

			// Add __extends to the constructor
			newObj.addConstructorLine(
					"__" + newObj.getName() + "__obj->__extends = " + splitResult[3] + "_constructor();");
		}
		resetClass(); // reset variables used by the previous class
	}

	private void resetClass() {
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
				className + "_t* " + "__" + className + "__obj = " + "malloc(sizeof(" + className + "_t));");
		current.addConstructorLineAt(0,
				className + "_t* " + className.concat("_").concat(constructorIdentifier) + argsWithParentheses + "{");

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

		String parameters = "(" + current.getName() + "_t* __" + current.getName() + "__obj";
		if (splitSpecial(',', code.substring(code.indexOf('(') + 1, code.lastIndexOf(')'))).length >= 1) {
			parameters += ",";
		}
		parameters += splitResult[0].substring(splitResult[0].indexOf('(') + 1);

		if (functionName.contentEquals("main")) {
			className = "";
			parameters = splitResult[0].substring(indexOfSpecial('(', splitResult[0]));
		}
		current.addVariable(returnType + "(*" + functionName + ") " + parameters + ";");
		current.addConstructorLine(
				"__" + current.getName() + "__obj->" + functionName + "=&" + className + functionName + ";");

		String completeHeader = returnType + " " + className
				+ functionHeader.substring(0, indexOfSpecial('(', functionHeader)) + parameters;
		current.addFunctionPrototype(completeHeader + ";");
		current.add(completeHeader + "{");

		// Change the current parentheses scope status to 1
		scopeStatus++;
	}

	private void executeIfFunction(String code) {
		// if 5 == 5 then
		String ifheader = "if (" + code.substring("if".length() + 2, code.lastIndexOf("then")).replaceAll("\\.", "->")
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
			// TODO: change .c to .h and move stuff to header files soon
			String header = HorseClass.INCLUDE + " \"" + thingToImport
					+ (isExistingClass(thingToImport) ? "__CLASS" : "") + ".h\"\n";
			;
			headers += header;
		}
	}

	private boolean isExistingClass(String name) {
		for (HorseClass cls : objects)
			if (name.contentEquals(cls.getName()))
				return true;
		return false;
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
			current.setEndOfConstructor("return __" + current.getName() + "__obj;\n}\n");
			inConstructor = false;
			scopeStatus--;
			return;
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
			if (letter == '"' || letter == '\'' || letter == '(' || letter == ')') {
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