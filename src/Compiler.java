import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class Compiler {

	// oficially started 6:35
	private String sourceFile;

	// string is char pointer
	private String[] c_identifers = { "basic_string", "basic_file" };
	private String[] c_functions = { "basic_printf", "basic_fopen", "basic_fputs" };

	private String headers;
	private String mainFunction;
	private String endOfMainFunction;

	// Inherits the C stuff and makes it safer from there
	public Compiler(String sourceFile) {
		this.sourceFile = sourceFile;

		// All the good string stuff
		headers = "#include <stdio.h>\n" + "#include <stdlib.h>\n" + "#include <stdbool.h>\n";
		mainFunction = "int main(int argc, char **argv) {\n";
		endOfMainFunction = "return 0;\n}";
	}

	// c_printf()
	public void start(String destFile) throws FileNotFoundException, IOException {
		try (BufferedReader r = new BufferedReader(new FileReader(sourceFile))) {
			String line;
			while ((line = r.readLine()) != null) {
				String cleanOne = cleanLine(line);

				if (cleanOne.isBlank())
					continue;

				if (cleanOne.startsWith("if")) {
					executeIfFunction(cleanOne);

				} else if (cleanOne.startsWith("for")) {
					executeForFuction(cleanOne);

				} else if (cleanOne.startsWith("end")) {
					mainFunction += "}\n";
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

		try (FileWriter w = new FileWriter(destFile)) {
			w.append(headers + mainFunction + endOfMainFunction);
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

	private void executeIfFunction(String code) {
		mainFunction += "if (" + code.substring("if".length(), code.lastIndexOf("then")) + ") {\n";
	}

	private void executeForFuction(String code) {
		mainFunction += "for (" + code.substring("for".length()).replaceAll(",", ";") + ") {\n";
	}

	private void executeCFunction(String code) {
		System.out.println(code);
		mainFunction += code.substring("basic_".length(), code.indexOf("->")) + "("
				+ code.substring(code.indexOf("->") + "->".length()) + ");\n";
	}

	private void executeCIdentifer(String identifer, String code) {
		if (identifer.contentEquals("basic_string") || identifer.contentEquals("basic_file")) {
			mainFunction += identifer + " *" + code.substring(identifer.length(), code.indexOf('=')) + " = ";
			executeCFunction(code.substring(code.indexOf('=') + 1, code.length()));
		}
	}
}