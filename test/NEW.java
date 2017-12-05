public class NEW{
	//public static int[] st = new int[2];
	public static int ss = 1;
	public static String str = "test";
	public static void main(String[] args){
		ss = 2;
	//	st = new int[3];
		//primitive type
		int[] i = new int[2];
		i[0] = 1;
		System.out.println(i[0]);
		long[] l = new long[2];
		l[0] = 2l;
		System.out.println(l[0]);
		float[] f = new float[1];
		f[0] = 3.0f;
		System.out.println(f[0]);
		double[] d = new double[3];
		d[0] = 4.0;
		System.out.println(d[0]);
		byte[] b = new byte[2];
		b[0] = 0x5;
		System.out.println(b[0]);
		short[] s = new short[2];
		s[0] = 6;
		System.out.println(s[0]);
		char[] c = new char[1];
		c[0] = '7';
		System.out.println(c[0]);
		boolean[] bl = new boolean[3];
		bl[0] = true;
		System.out.println(bl[0]);
		int[][] a = new int[3][5];
		a[2][3] = 13;
		System.out.println(a[2][3]);
		
		int[][][] aa = new int[3][4][5];
		aa[1][2][3] = 10000;
		System.out.println(aa[1][2][3]);
		print();
		print(aa);
		
		/////////////////////////
		NEW n = new NEW();
		////////////////////////
		NEW[] nn = new NEW[3];
		nn [0] = n;
		NEW[][] nn2 = new NEW[5][3];
		nn2[1][2] = n;
		
	}

	public static void print(){
		System.out.println(str);
	}
	
	public static void print(int[][][] a){
		System.out.println(a[1][2][3]);
	}
}
