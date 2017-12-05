public class TestInvoke{
	public static void main(String[] args){
		TestInstance instance = new TestInstance("test",30);
		instance.printName();
	}
}

class TestInstance{
	private String name;
	private int age;

	public TestInstance(String name,int age){
		this.name = name;
		this.age = age;
	}

	public void printName(){
		System.out.println(this.name);
	}
}
