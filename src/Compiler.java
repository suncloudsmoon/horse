import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Compiler {
	
	private static final String HORSE_EXTENSION = ".hs";

	// oficially started 6:35
	private String sourceFile;

	private String classIdentifier = "class";
	private String longIndentifier = "long";
	private String voidIdentifier = "void";
	private String functionIdentifer = "function";
	private String returnIdentifier = "return";
	
	private String[] c_identifers = { "unsafe_string", "unsafe_file" };
	private String[] c_functions = { "unsafe_printf", "unsafe_fopen", "unsafe_fputs" };

	private String headers;

	private List<HorseClass> objects;
	
	private int scopeStatus; // 0 = class, 1 = function

	// Inherits the C stuff and makes it safer from there
	public Compiler(String sourceFile) {
		this.sourceFile = sourceFile;

		headers = "#include <stdio.h>\n" + "#include <stdlib.h>\n" + "#include <stdbool.h>\n";
		objects = new ArrayList<>();
		scopeStatus = 0;
	}

	// c_printf()
	public void start(String destFile) throws FileNotFoundException, IOException {
		try (BufferedReader r = new BufferedReader(new FileReader(sourceFile))) {
			String line;
			while ((line = r.readLine()) != null) {
				String cleanOne = cleanLine(line);

				if (cleanOne.isBlank())
					continue;
				
				System.out.println(cleanOne);

				if (cleanOne.startsWith(classIdentifier)) {
					executeClass(cleanOne);
					
				} else if (cleanOne.startsWith("function")) { 
					executeFunctionDeclaration(cleanOne);
					
				} else if (cleanOne.startsWith(returnIdentifier)) {
					executeReturnIndentifer(cleanOne);
					
				} else if (cleanOne.startsWith("if")) {
					executeIfFunction(cleanOne);

				} else if (cleanOne.startsWith("for")) {
					executeForFuction(cleanOne);

				} else if (cleanOne.startsWith("end")) {
					executeEndIndentifer();

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
			try (FileWriter w = new FileWriter("test/" + cls.getName() + ".c")) {
				w.append(cls.getPackage());
			}
		}
	}

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
	
	private void executeClass(String code) {
		String className = code.substring(classIdentifier.length());
		// Adds a new object to the list
		objects.add(new HorseClass(className, headers));
	}
	
	private void executeFunctionDeclaration(String code) {
		// function sayHello() returns String
		String parts[] = code.split("returns");
		String functionName = parts[0].substring(functionIdentifer.length());
		String returnType;
		
		// if it is a long, or any other primitive, then there should be no pointer
		if (parts[1].contentEquals(voidIdentifier) || parts[1].contentEquals(longIndentifier)) {
			returnType = parts[1];
		} else {
			returnType = parts[1] + "*";
		}
		 
		// void sayHello();
		String cFunctionHeader = returnType + " " + objects.get(objects.size() - 1).getName() + "_" + functionName + "{\n";
		objects.get(objects.size() - 1).add(cFunctionHeader);
		
		// Change the current parentheses scope status to 1
		scopeStatus = 1;
	}

	private void executeIfFunction(String code) {
		String compiled = "if (" + code.substring("if".length(), code.lastIndexOf("then")) + ") {\n";
		objects.get(objects.size() - 1).add(compiled);
	}

	private void executeForFuction(String code) {
		String compiled = "for (" + code.substring("for".length()).replaceAll(",", ";") + ") {\n";
		objects.get(objects.size() - 1).add(compiled);
	}

	private void executeCFunction(String code) {
		String compiled = code.substring(code.indexOf("_") + 1) + ";\n";
		objects.get(objects.size() - 1).add(compiled);
	}

	private void executeCIdentifer(String identifer, String code) {
		String compiled = code.substring(0, code.indexOf('=')) + ";";
		objects.get(objects.size() - 1).add(compiled);
		executeCFunction(code.substring(code.indexOf('=') + 1, code.length()));
	}
	
	private void executeReturnIndentifer(String code) {
		objects.get(objects.size() - 1).add(returnIdentifier + " " + code.substring(returnIdentifier.length()) + ";\n");
	}
	
	private void executeEndIndentifer() {
		if (scopeStatus == 1)
			objects.get(objects.size() - 1).add("}\n");
		scopeStatus = 0;
	}
}