import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

public class HorseClass {

	public static final String IFDEF = "#ifdef", ENDIF = "#endif", DEFINE = "#define";
	public static final String INCLUDE = "#include";
	public static final String CLASS = "__CLASS";

	private String name;

	// Headers, etc.
	private String headers;

	private List<String> variableIndexList;
	private List<String> compiledVariables;

	private List<String> compiledLines;
	private List<String> constructorLines;
	private String endOfConstructor;

	private List<String> functionPrototypes;

	public HorseClass(String name, String headers) {
		this.name = name;
		this.headers = headers;

		// Initializing new objects...
		compiledVariables = new LinkedList<>();
		variableIndexList = new LinkedList<>();

		compiledLines = new LinkedList<>();
		constructorLines = new LinkedList<>();

		functionPrototypes = new LinkedList<>();
	}

	public void add(String compiledLine) {
		compiledLines.add(compiledLine);
	}

	/**
	 * Can map the function calls here too
	 * 
	 * @param line
	 */
	public void addConstructorLine(String line) {
		constructorLines.add(line);
	}

	public void addConstructorLineAt(int index, String line) {
		constructorLines.add(index, line);
	}

	public void addFunctionPrototype(String line) {
		functionPrototypes.add(line);
	}

	/**
	 * Can also add function names (like C where you can treat functions like
	 * pointers)
	 * 
	 * @param var
	 */
	public void addVariable(String var) {
		compiledVariables.add(var);
	}

	public void addVariableIndex(String varName) {
		variableIndexList.add(varName);
	}

	public String getSourceFilePackage() {
		String includeFile = INCLUDE + " \"" + name + CLASS + ".h\"";
		return includeFile + "\n" + getConstructor() + "\n" + getRestOfTheLines();
	}

	public String getHeaderFilePackage() {
		// Pre-Definitions to make life easier :)
		String ifdef = IFDEF + " " + name + "_H_";
		String define = DEFINE + " " + name + "_H_";
		String endIf = ENDIF + "\n";

		String startHeader = ifdef + "\n" + define;
		String endHeader = endIf;

		// We set headers to null frequently, so the if check is necessary
		if (headers == null)
			headers = "";

		return startHeader + "\n" + headers + "\n" + getAllAttributes() + "\n" + getAllFunctionPrototypes() + "\n"
				+ endHeader;
	}

	private String getAllAttributes() {
		String fullTypedef = "typedef struct " + name + "_t " + name + "_t;\n";
		fullTypedef += "struct " + name + "_t{\n";
		for (String var : compiledVariables) {
			fullTypedef += var + "\n";
		}
		fullTypedef += "\n};";
		return fullTypedef;
	}

	private String getAllFunctionPrototypes() {
		String allPrototypes = "";
		for (String funct : functionPrototypes) {
			allPrototypes += funct + "\n";
		}
		return allPrototypes;
	}

	private String getConstructor() {
		String fullSource = "";
		for (String line : constructorLines)
			fullSource += line + "\n";
		fullSource += endOfConstructor;
		return fullSource;
	}

	private String getRestOfTheLines() {
		String fullSource = "";
		for (String line : compiledLines) {
			fullSource += line + "\n";
		}
		return fullSource;
	}

	public void writeToFile(String initialDirectory) throws IOException {
		try (FileWriter w = new FileWriter(initialDirectory + name + CLASS + ".c")) {
			w.append(getSourceFilePackage());
		}

		try (FileWriter w = new FileWriter(initialDirectory + name + CLASS + ".h")) {
			w.append(getHeaderFilePackage());
		}
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * @return the endOfConstructor
	 */
	public String getEndOfConstructor() {
		return endOfConstructor;
	}

	/**
	 * @param endOfConstructor the endOfConstructor to set
	 */
	public void setEndOfConstructor(String endOfConstructor) {
		this.endOfConstructor = endOfConstructor;
	}

	/**
	 * @return the variableIndexList
	 */
	public List<String> getVariableIndexList() {
		return variableIndexList;
	}

}
