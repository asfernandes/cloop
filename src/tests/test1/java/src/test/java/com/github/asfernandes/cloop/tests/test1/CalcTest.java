/*
 *  The contents of this file are subject to the Initial
 *  Developer's Public License Version 1.0 (the "License");
 *  you may not use this file except in compliance with the
 *  License. You may obtain a copy of the License at
 *  http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *  Software distributed under the License is distributed AS IS,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied.
 *  See the License for the specific language governing rights
 *  and limitations under the License.
 *
 *  The Original Code was created by Adriano dos Santos Fernandes.
 *
 *  Copyright (c) 2015 Adriano dos Santos Fernandes <adrianosf at gmail.com>
 *  and all contributors signed below.
 *
 *  All Rights Reserved.
 *  Contributor(s): ______________________________________.
 */

package com.github.asfernandes.cloop.tests.test1;

import com.github.asfernandes.cloop.tests.test1.ICalc.*;

import com.sun.jna.Native;

import org.junit.Assert;
import org.junit.Test;


public class CalcTest
{
	@Test
	public void test() throws CalcException
	{
		Calc calc = (Calc) Native.loadLibrary("test1-c.so", Calc.class);
		IFactory factory = calc.createFactory();
		IStatus status = factory.createStatus();

		ICalculator calculator = factory.createCalculator(status);

		calculator.sumAndStore(status, 1, 22);
		Assert.assertEquals(calculator.getMemory(), 23);

		calculator.setMemory(calculator.sum(status, 2, 33));
		Assert.assertEquals(calculator.getMemory(), 35);

		ICalculator2 calculator2 = factory.createCalculator2(status);

		calculator2.copyMemory(calculator);
		Assert.assertEquals(calculator2.getMemory(), 35);

		int[] address = new int[] {40};
		calculator2.copyMemory2(address);
		Assert.assertEquals(calculator2.getMemory(), 40);

		calculator.dispose();
		calculator = calculator2;

		calculator.sumAndStore(status, 1, 22);
		Assert.assertEquals(calculator.getMemory(), 23);

		calculator.setMemory(calculator.sum(status, 2, 33));
		Assert.assertEquals(calculator.getMemory(), 35);

		calculator2.sumAndStore(status, 1, 22);
		Assert.assertEquals(calculator2.getMemory(), 23);

		calculator2.setMemory(calculator2.multiply(status, 2, 33));
		Assert.assertEquals(calculator2.getMemory(), 66);

		calculator.dispose();

		calculator = factory.createBrokenCalculator(status);

		calculator.sumAndStore(status, 1, 22);
		Assert.assertEquals(calculator.getMemory(), 24);

		calculator.setMemory(calculator.sum(status, 2, 33));
		Assert.assertEquals(calculator.getMemory(), 36);

		int code = -1;
		try
		{
			calculator.sum(status, 600, 600);
		}
		catch (CalcException e)
		{
			code = e.getCode();
		}

		Assert.assertEquals(code, IStatus.ERROR_1);

		calculator.dispose();
		factory.dispose();
	}
}
