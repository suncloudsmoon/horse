import java.util.LinkedList;
import java.util.List;

public class HorseClass {

	private String name;

	// Headers, etc.
	private String header;

	private List<String> variableIndexList;
	private List<String> compiledVariables;

	private List<String> compiledLines;
	private List<String> constructorLines;
	private String endOfConstructor;

	public HorseClass(String name, String header) {
		this.name = name;
		this.header = header;

		// Initializing new objects...
		compiledVariables = new LinkedList<>();
		variableIndexList = new LinkedList<>();

		compiledLines = new LinkedList<>();
		constructorLines = new LinkedList<>();
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

	public String getPackage() {
		// For now, getConstructor() is at the last line so we don't have to write prototypes
		// TODO: fix this method
		 return header + "\n" + getAllAttributes() + "\n" + getRestOfTheLines() + "\n" + getConstructor();
		// return header + "\n" + getAllAttributes() + "\n" + getConstructor() + "\n" + getRestOfTheLines() + "\n";
	}

	private String getAllAttributes() {
		String fullTypedef = "typedef struct {\n";
		for (String var : compiledVariables) {
			fullTypedef += var + "\n";
			System.out.println("var: " + var + "\n");
		}
		fullTypedef += "\n} " + name + "_t;\n";
		return fullTypedef;
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
			System.out.println("getRestOfTheLines(): " + line);
		}
			

		return fullSource;
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
