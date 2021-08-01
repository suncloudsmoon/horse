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
	private String longIdentifier = "long";
	private String classIdentifier = "class";
	private String voidIdentifier = "void";
	private String functionIdentifer = "function";
	private String returnIdentifier = "return";

	// C Identifiers
	private String[] c_identifers = { "unsafe_char*", "unsafe_FILE", "long" };
	private String[] c_functions = { "unsafe_printf", "unsafe_fopen", "unsafe_fputs" };

	// Import stuff
	private static final String IMPORT_BASICS = "#include <stdio.h>\n" + "#include <stdlib.h>\n"
			+ "#include <stdbool.h>\n";

	// Others
	private String headers;
	private int scopeStatus; // 0 = class, 1 = function

	private List<HorseClass> objects;

	// Inherits the C stuff and makes it safer from there
	public Compiler(String sourceFile) {
		this.sourceFile = sourceFile;

		objects = new ArrayList<>();

		// Initializes the header string
		headers = "";
		scopeStatus = 0;
	}

	// c_printf()
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
					for (String cfunc : c_functions)
						if (cleanOne.startsWith(cfunc))
							executeCFunction(cleanOne);

					for (String cIdent : c_identifers)
						if (cleanOne.startsWith(cIdent))
							executeCIdentifer(cIdent, cleanOne);
				}
			}
		}

		for (HorseClass cls : objects) {
			try (FileWriter w = new FileWriter(Main.testOrNot + cls.getName() + ".c")) {
				w.append(cls.getPackage());
			}
		}
	}

	// Class/Object Functions
	private void executeClassIdentifier(String code) {
		String className = code.substring(classIdentifier.length(), code.indexOf("extends"));
		String afterExtends = "";

		// Adds a new object to the list
		objects.add(new HorseClass(className, headers));

		if (code.contains("extends")) {
			afterExtends += code.substring(code.indexOf("extends") + "extends".length());
			objects.get(objects.size() - 1).addVariable(afterExtends + " *__extends;");
		}

		// Reset all indicators now on
		// Reset headers, so new classes start from scratch
		headers = "";
		scopeStatus = 0;
	}

	private void executeConstructor(String code) {
		// constructor Hello() -> name of the class
		String className = objects.get(objects.size() - 1).getName();
		String argsWithParentheses = code.substring(constructorIdentifier.length());
		objects.get(objects.size() - 1).add(
				className + "* " + className.concat("_").concat(constructorIdentifier) + argsWithParentheses + "{");

		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private void executeFunctionDeclaration(String code) {
		// function sayHello() returns String
		String parts[] = code.split("returns");
		String functionName = parts[0].substring(functionIdentifer.length());
		String returnType;

		// if it is a long, or any other primitive, then there should be no pointer
		if (parts[1].contentEquals(voidIdentifier) || parts[1].contentEquals(longIdentifier)) {
			returnType = parts[1];
		} else {
			returnType = parts[1] + "*";
		}

		// void sayHello();
		String cFunctionHeader = returnType + " " + objects.get(objects.size() - 1).getName() + "_" + functionName
				+ "{\n";
		objects.get(objects.size() - 1).add(cFunctionHeader);

		// Change the current parentheses scope status to 1
		scopeStatus++;
	}

	private void executeIfFunction(String code) {
		String ifheader = "if (" + code.substring("if".length(), code.lastIndexOf("then")) + ") {";
		objects.get(objects.size() - 1).add(ifheader);

		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private void executeForFuction(String code) {
		String forheader = "for (" + code.substring("for".length()).replaceAll(",", ";") + ") {";
		objects.get(objects.size() - 1).add(forheader);

		// Parentheses adjuster to account for the end of class definition
		scopeStatus++;
	}

	private void executeCFunction(String code) {
		String functionHeader = code.substring(code.indexOf("_") + 1) + ";";
		objects.get(objects.size() - 1).add(functionHeader);
	}

	private void executeCIdentifer(String identifier, String code) {
		String varName, varDeclaration;
		if (code.contains("=")) {
			varName = code.substring(identifier.length(), code.indexOf('='));
			varDeclaration = code.substring(0, code.indexOf('=')) + ";";
			// Add the variable to the list & index
			objects.get(objects.size() - 1).addVariableIndex(varName.substring(1));
			objects.get(objects.size() - 1).addVariable(varDeclaration);

			if (code.contains("(") && code.contains(")"))
				executeCFunction(code.substring(code.indexOf('=') + 1, code.length()));

		} else {
			varName = code.substring(identifier.length());
			varDeclaration = code + ";";

			// It means that there was something after the declaration
			// unsafe_char *name = "Hello, World"
			objects.get(objects.size() - 1).addVariableIndex(varName.substring(1));
			objects.get(objects.size() - 1)
					.addVariable(identifier + " " + code.substring(identifier.length()) + ";");
		}
	}

	private void executeImportIdentifier(String code) {
		String thingToImport = code.substring(importIdentifier.length());
		if (thingToImport.contentEquals("basics")) {
			headers += IMPORT_BASICS;
		} else {
			headers += "#include <" + thingToImport + ".h>";
		}

		System.out.println(headers);

	}

	private void executeReturnIdentifier(String code) {
		objects.get(objects.size() - 1).add(returnIdentifier + " " + code.substring(returnIdentifier.length()) + ";");
	}

	private void executeEndIdentifier() {
		if (scopeStatus >= 1)
			objects.get(objects.size() - 1).add("}");

		if (scopeStatus - 1 >= 0)
			scopeStatus--;
	}

	/**
	 * Cleans the line of any spaces if it is not in a quote
	 * 
	 * @param originalLine
	 * @return
	 */
	private String cleanLine(String originalLine) {
		String newStr = "";
		boolean inQuote = false;
		for (int i = 0; i < originalLine.length(); i++) {
			char letter = originalLine.charAt(i);
			if (letter == '"' || letter == '\'') {
				inQuote = !inQuote;
				newStr += letter;
			} else if (!inQuote && letter != ' ' && letter != '\t') {
				newStr += letter;

			} else if (inQuote) {
				newStr += letter;
			}
		}
		return newStr;
	}
}