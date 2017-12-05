public class ArrayInstanceofMain{
	public static void main(String[] args){
		int[] a1 = new int[2];
		if(a1 instanceof int[]){//true
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		Interface1[] in1 = new Interface1Impl[2];
		if(in1 instanceof Interface2[]){//false
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		in1 = new Interface2Impl2[2];
		if(in1 instanceof Interface1[]){//true
			System.out.println("true");
		}else{
			System.out.println("false");
		}

		Interface2Impl2[] l2 = new Interface2Impl2[2];
		if(l2 instanceof Interface2Impl[]){//true
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		
		Interface2Impl[] il2 = new Interface2Impl[2];
		if(il2 instanceof Interface2Impl2[]){//false
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		
		
		Interface1[][] ii1 = new Interface1Impl[2][2];
		if(ii1 instanceof Interface2Impl[][]){//false
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		
		Interface2[] t = new Interface2[2];		
		if(t instanceof Interface1[]){//true
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		
	}
}
