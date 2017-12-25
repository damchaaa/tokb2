#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include "Form1.h"
#include "Form2.h"
#include <vcclr.h>
#include <windows.h>
using namespace std;
using namespace System;
using namespace System::Windows::Forms;



HANDLE openDeviceForRead()//������� ���������� ��� ������
{
	HANDLE handle;
	handle = CreateFile(L"\\\\.\\F:",
		GENERIC_READ, 0,
		NULL, OPEN_EXISTING, 0, NULL);
	if (handle != INVALID_HANDLE_VALUE)
	{
		return handle;
	}
	else
	{
		MessageBox::Show("���������� �� �������!!",
			"������!", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
		//CloseHandle(handle);
		Application::Exit();
	}
}

HANDLE openDeviceForWrite()//������� ���������� ��� ������
{
	HANDLE handle;
	handle = CreateFile(L"\\\\.\\F:",
		GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, 0, NULL);
	if (handle != INVALID_HANDLE_VALUE)
	{
		return handle;
	}
	else
	{
		MessageBox::Show("���������� �� �������!!",
			"������!", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
		//CloseHandle(handle);
		Application::Exit();
	}
}

void InitSector(HANDLE device, DWORD sector, char *pass)//������ � ������ � ������
{
	const int numberofsectors = 1;
	SetFilePointer(device, (sector * 512), NULL, FILE_BEGIN);
	DWORD dwBytesRead;

	char buffer[512];
	if (ReadFile(device, buffer, 512 * numberofsectors, &dwBytesRead, NULL))//���� ������ �������
	{
		//���������� � 54 ������� ������
		int sum = 0;
		int countSymbs = 0;
		for (int i = 54; i <= 54 + strlen(pass); i++)
		{
			if (buffer[i] == pass[sum])
			{
				countSymbs++;
			}
			else
			{
				buffer[i] = pass[sum];

			}
			sum++;
		}
		CloseHandle(device);
		HANDLE hDevice1 = openDeviceForWrite();
		if (hDevice1 != INVALID_HANDLE_VALUE)
		{
			if (WriteFile(device, buffer, 512 * numberofsectors, &dwBytesRead, NULL))// ���� ������ �������
			{
				MessageBox::Show("������ ������� ������� �� ����������!!!",
					"������!!", MessageBoxButtons::OK,
					MessageBoxIcon::Asterisk);

				CloseHandle(device);
			}
		}
	}
}
bool CheckPass(const char *pass)//�������� ������ �� �������
{
	int count = 0;
	for (int i = 0; i < strlen(pass); i++)
	{
		if (pass[i] == ' ')
			count++;
	}
	if (count != strlen(pass))
		return true;
	else
		return false;
}
void WritePassToNullSector(char *pass)//������ ������ � ������� ������ ����������
{
	HANDLE hDevice = openDeviceForRead();
	//��������� ���������� ��� ������
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		InitSector(hDevice, 0, pass);
	}
}

char * TOKB_LAB2::Form1::loopRecord(char *pass, const char *key)//����������� ������ �����
{
	char * result = (char *)malloc(sizeof(char)*strlen(pass));
	int lenmass = 0;
	int numSymb = 0;
	for (int i = 0; i < strlen(pass); i++)
	{
		result[i] = key[lenmass];
		lenmass++;
		numSymb++;
		if (lenmass == strlen(key))
		{
			lenmass = 0;
		}
	}
	result[numSymb] = '\0';
	return result;
	free(result);
}

char * TOKB_LAB2::Form1::strCpy(char *str1, char *str2)//����������� ����� ������ � ������
{
	int i = 0;
	while (str2[i] != '\0')
	{
		str1[i] = str2[i];
		i++;
	}
	str1[i] = '\0';
	return str1;
}

char * TOKB_LAB2::Form1::Encryption(char * input, const char * key)//���� ��������
{
	char latticeVizh[26][26];
	int offSet = 0;
	int firstSymb = 97;//��������� ������ �� ��������
	int lastSymb = 122;//�������� ������ �� ��������
	for (int i = 0; i < 26; i++)
	{
		int count = firstSymb;
		int start = firstSymb;//������� ��� ������
		for (int j = 0; j < 26; j++)
		{
			latticeVizh[i][j] = (char)(count + offSet);
			count++;
			if ((int)latticeVizh[i][j] > lastSymb)//���� ������� �� ����� �������� �������� ��������
			{
				latticeVizh[i][j] = (char)start;
				start++;
			}
			if ((int)latticeVizh[i][j] < 0)
			{
				latticeVizh[i][j] = (char)start;
				start++;
			}
		}
		offSet++;
	}
	char * pass = (char *)malloc(sizeof(char)*strlen(input));
	strCpy(pass, input);
	char * output = (char *)malloc(sizeof(char)*strlen(pass));
	output = loopRecord(pass, key);
	char * cipher = (char *)malloc(sizeof(char)*strlen(pass));
	int numSymb = 0;
	for (int i = 0; i < strlen(pass); i++)
	{
		cipher[i] = latticeVizh[(int)(pass[i] - firstSymb)][(int)(output[i] - firstSymb)];//�� ������ � ������� ���������� ����������� ������
		numSymb++;
	}
	cipher[numSymb] = '\0';

	return cipher;
	free(pass);
	free(output);
	free(cipher);
}


char * TOKB_LAB2::Form1::StringToChar(String^ str)//String^ to �har
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	errno_t err = 0;
	char    *ch = (char *)malloc(sizeInBytes);

	err = wcstombs_s(&convertedChars,
		ch, sizeInBytes,
		wch, sizeInBytes);
	return ch;
}
const char * strToConstChar(string str)//string to char
{
	const char * result = str.c_str();
	return result;
}



void WriteToFileAndSector(char *str)//������ ������ � ������
{
	if (CheckPass(str))
	{
		WritePassToNullSector(str);
	}
	else
	{
		MessageBox::Show("������ ������.�������� ����� ������!!",
			"������!", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
		Application::Exit();
	}

}

System::Void TOKB_LAB2::Form1::button1_Click(System::Object^  sender, System::EventArgs^  e)//���� ������ Form1
{
	int NumPass = 3;//���������� ������� ��� ����� ������
	char * key = "lemon";//����
	char * passFromTextBox = Encryption(StringToChar(textBox1->Text), key);
	char pass[9];
	char password[9];
	//������
	HANDLE hDevice = openDeviceForRead();
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		const int numberofsectors = 1;
		SetFilePointer(hDevice, (0 * 512), NULL, FILE_BEGIN);
		DWORD dwBytesRead;

		char buffer[512];

		if (ReadFile(hDevice, buffer, 512 * numberofsectors, &dwBytesRead, NULL))//���� ������ �������
		{
			int sum = 0;
			int count = 0;
			char pass[9];
			for (int i = 54; i <= 61; i++)
			{
				if (buffer[i] != NULL)
				{
					pass[sum] = buffer[i];
					sum++;
				}
				else
				{
					count++;

				}

			}
			
			if (count == 8)
			{
				CloseHandle(hDevice);
				strcpy(password, " ");
			}
			else
			{
				if (CheckPass(pass))
				{
					CloseHandle(hDevice);
					pass[sum] = '\0';
					strcpy(password, pass);
				}
			}
		}
		else
		{
			MessageBox::Show("���������� �� �������!!",
				"������!", MessageBoxButtons::OK,
				MessageBoxIcon::Asterisk);
			CloseHandle(hDevice);
			strcpy(password, " ");

		}

		if (strcmp(password," ")==0)//���� ���� ����������
		{
			WriteToFileAndSector(passFromTextBox);//���������� ������ 
			MessageBox::Show("������ �������",
				"������ ������", MessageBoxButtons::OK,
				MessageBoxIcon::Asterisk);
		}
		else//���������� � ������
		{
			//CloseHandle(hDevice);
			if (strcmp(password, passFromTextBox) == 0)//���� ����������� ��������� ������ ��������� � ����������� � �����
			{
				MessageBox::Show("������ ������",
					"�������� ������", MessageBoxButtons::OK,
					MessageBoxIcon::Asterisk);
				HWND h;
				ShellExecute(h, L"open", L"F:" , NULL, NULL, SW_SHOWNORMAL);

				button2->Enabled = true;
			}
			else//���� �� ��������� 
			{
				button2->Enabled = false;

				NumPass--;
				if (NumPass == 0)//������������ ���������� �������
				{
					MessageBox::Show("�� ��������� ���������� �������",
						"�������� ������", MessageBoxButtons::OK,
						MessageBoxIcon::Asterisk);
					Application::Exit();
				}
				String^ numpass = "�������� ������� - " + NumPass.ToString();
				MessageBox::Show(numpass,
					"�������� ������!", MessageBoxButtons::OK,
					MessageBoxIcon::Asterisk);
			}

			//textBox1->Enabled = false;
		}
	}
	else
	{
		MessageBox::Show("������ ������.�������� ����� ������!!",
			"������!", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
		CloseHandle(hDevice);
		WriteToFileAndSector(passFromTextBox);//���������� ������ 
		MessageBox::Show("������ �������",
			"������ ������", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
	}

}
System::Void TOKB_LAB2::Form1::textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e)//���� ������ Form1
{
	if (textBox1->Text->Length > 8)
		textBox1->Text = "";
}

System::Void TOKB_LAB2::Form1::button2_Click(System::Object^  sender, System::EventArgs^  e)//����� Form2 ��� ����� ������
{
	Form2^ fr2 = gcnew Form2;
	fr2->Show();
}

System::Void TOKB_LAB2::Form2::textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) //������ ������ Form2
{
	if (textBox1->Text->Length > 8)
		textBox1->Text = "";
}

System::Void TOKB_LAB2::Form2::button1_Click(System::Object^  sender, System::EventArgs^  e) //������� ������ Form2
{
	int NumPass = 3;
	char * key = "lemon";//����

	char * oldPassFromTextBox = Form1::StringToChar(Form2::textBox1->Text);//������ ������

	char * EncryptedOldPass = Form1::Encryption(oldPassFromTextBox, key);//������������� ������ ������

	char pass[9];
	char password[9];
	HANDLE hDevice = openDeviceForRead();
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		const int numberofsectors = 1;
		SetFilePointer(hDevice, (0 * 512), NULL, FILE_BEGIN);
		DWORD dwBytesRead;

		char buffer[512];

		if (ReadFile(hDevice, buffer, 512 * numberofsectors, &dwBytesRead, NULL))//���� ������ �������
		{
			int sum = 0;
			int count = 0;
			char pass[9];
			for (int i = 54; i <= 61; i++)
			{
				if (buffer[i] != NULL)
				{
					pass[sum] = buffer[i];
					sum++;
				}
				else
				{
					count++;

				}

			}
			pass[sum] = '\0';
			if (count == 9)
			{
				CloseHandle(hDevice);
				strcpy(pass, NULL);
			}
			else
			{
				if (CheckPass(pass))
				{
					CloseHandle(hDevice);

					strcpy(password, pass);
					if (password != NULL)
					{
						//char * EncryptedPassFromSector = Form1::Encryption(password, key);
						if (strcmp(password, EncryptedOldPass) == 0)//���� ������ ������ ��������� � ������������� � �����
						{
							char * newPassFromTextBox = Form1::StringToChar(Form2::textBox2->Text);//����� ������
							char * EncryptedNewPass = Form1::Encryption(newPassFromTextBox, key);//����������� ����� ������

							char * newPassConfirm = Form1::StringToChar(Form2::textBox3->Text);//������������� ������ ������
							if (strcmp(newPassFromTextBox, newPassConfirm) == 0)//���� ����� ������ �����������
							{
								WriteToFileAndSector(EncryptedNewPass);
								MessageBox::Show("�������� ����� ������ �������!", "������ �������", MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
							}
							else//���� ������ �� �����������
							{
								MessageBox::Show("����������� ������ ��� ���!",
									"�������� ������", MessageBoxButtons::OK,
									MessageBoxIcon::Asterisk);
								Form2::textBox3->Text = "";
							}
						}
						else//���� ������ �� ���������
						{
							NumPass--;
							if (NumPass == 0)
							{
								MessageBox::Show("�� ��������� ���������� �������",
									"�������� ������", MessageBoxButtons::OK,
									MessageBoxIcon::Asterisk);
								exit(1);
							}
							String^ numpass = "�������� ������� - " + NumPass.ToString();
							MessageBox::Show("�������� ������ ������!",
								numpass, MessageBoxButtons::OK,
								MessageBoxIcon::Asterisk);
							textBox1->Text = "";
						}
					}
				}
				else
				{
					MessageBox::Show("������ ������.�������� ����� ������!!",
						"������!", MessageBoxButtons::OK,
						MessageBoxIcon::Asterisk);
					CloseHandle(hDevice);
					strcpy(pass, NULL);
				}
			}
		}
	}
	else
	{
		MessageBox::Show("���������� �� �������!!",
			"������!", MessageBoxButtons::OK,
			MessageBoxIcon::Asterisk);
		CloseHandle(hDevice);
		strcpy(pass, NULL);

	}
	//���� ������ ������� �� ����������

}

System::Void TOKB_LAB2::Form2::textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) // ����� ������ Form2
{
	if (textBox2->Text->Length > 8)
		textBox2->Text = "";
}

System::Void TOKB_LAB2::Form2::textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) //������������� ������ ������ Form2
{
	if (textBox2->Text->Length > 8)
		textBox2->Text = "";
}