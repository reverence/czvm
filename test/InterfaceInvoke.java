public class InterfaceInvoke{
	public static void main(String[] args){
		TestInterface t1 = new MyInterfaceImpl();
		t1.print("my interfaceImpl");
		t1 = new OtherInterfaceImpl();
		t1.print("other interfaceImpl");	
		
		TestInterface2 t2 = new MyInterfaceImpl2();
		t2.print("my interfaceImpl2");
		t2 = new OtherInterfaceImpl2();
		t2.print("other interfaceImpl2");
	}
}

interface TestInterface{
	public void print(String message);
}

class MyInterfaceImpl implements TestInterface{
	public void print(String message){
		System.out.println(message);
	}
}

class OtherInterfaceImpl implements TestInterface{
	public void print(String message){
		System.out.println(message);
	}
}

interface TestInterface2{
	public void print(String message);
}

class MyInterfaceImpl2 implements TestInterface2{
	public void print(String message){
		System.out.println(message);
	}
}

class OtherInterfaceImpl2 implements TestInterface2{
	public void print(String message){
		System.out.println(message);
	}
}
