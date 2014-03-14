/*
 * Written by Thomas Heller, May 2000
 *
 * $Id: install.c,v 1.11 2001/04/09 18:03:11 theller Exp $
 */

/*
 * Windows Installer program for distutils.
 *
 * (a kind of self-extracting zip-file)
 *
 * At runtime, the exefile has appended:
 * - compressed setup-data in ini-format, containing the following sections:
 *	[metadata]
 *	author=Greg Ward
 *	author_email=gward@python.net
 *	description=Python Distribution Utilities
 *	licence=Python
 *	name=Distutils
 *	url=http://www.python.org/sigs/distutils-sig/
 *	version=0.9pre
 *
 *	[Setup]
 *	info= text to be displayed in the edit-box
 *	title= to be displayed by this program
 *	target_version = if present, python version required
 *	pyc_compile = if 0, do not compile py to pyc
 *	pyo_compile = if 0, do not compile py to pyo
 *
 * - a struct meta_data_hdr, describing the above
 * - a zip-file, containing the modules to be installed.
 *   for the format see http://www.pkware.com/appnote.html
 *
 * What does this program do?
 * - the setup-data is uncompressed and written to a temporary file.
 * - setup-data is queried with GetPrivateProfile... calls
 * - [metadata] - info is displayed in the dialog box
 * - The registry is searched for installations of python
 * - The user can select the python version to use.
 * - The python-installation directory (sys.prefix) is displayed
 * - When the start-button is pressed, files from the zip-archive
 *   are extracted to the file system. All .py filenames are stored
 *   in a list.
 */
/*
 * Includes now an uninstaller.
 */

/*
 * To Do:
 *  - install a help-button, which will display the above
 *    text to the user
 *  - should there be a possibility to display a README file
 *    before starting the installation (if one is present in the archive)
 *  - more comments about what the code does(?)
 *
 *  - evolve this into a full blown installer (???)
 */

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "archive.h"

/* Bah: global variables */
FILE *logfile;

char modulename[MAX_PATH];

HWND hwndMain;
HWND hDialog;

char *ini_file;			/* Full pathname of ini-file */
/* From ini-file */
char info[4096];		/* [Setup] info= */
char title[80];			/* [Setup] title=, contains package name
				   including version: "Distutils-1.0.1" */
char target_version[10];	/* [Setup] target_version=, required python
				   version or empty string */
char build_info[80];		/* [Setup] build_info=, distutils version
				   and build date */

char meta_name[80];		/* package name without version like
				   'Distutils' */



int py_major, py_minor;		/* Python version selected for installation */

char *arc_data;			/* memory mapped archive */
DWORD arc_size;			/* number of bytes in archive */
int exe_size;			/* number of bytes for exe-file portion */
char install_dir[MAX_PATH];
char pythondll[MAX_PATH];
BOOL pyc_compile, pyo_compile;

BOOL success;			/* Installation successfull? */

HANDLE hBitmap;
char *bitmap_bytes;


#define WM_NUMFILES WM_USER+1
	/* wParam: 0, lParam: total number of files */
#define WM_NEXTFILE WM_USER+2
	/* wParam: number of this file */
	/* lParam: points to pathname */

enum { UNSPECIFIED, ASK, ALWAYS, NEVER } allow_overwrite = UNSPECIFIED;

static BOOL notify (int code, char *fmt, ...);

static void unescape (char *dst, char *src, unsigned size)
{
    char *eon;
    char ch;

    while (src && *src && (size > 2)) {
	if (*src == '\\') {
	    switch (*++src) {
	    case 'n':
		++src;
		*dst++ = '\r';
		*dst++ = '\n';
		size -= 2;
		break;
	    case 'r':
		++src;
		*dst++ = '\r';
		--size;
		break;
	    case '0': case '1': case '2': case '3':
		ch = (char)strtol (src, &eon, 8);
		if (ch == '\n') {
		    *dst++ = '\r';
		    --size;
		}
		*dst++ = ch;
		--size;
		src = eon;
	    }
	} else {
	    *dst++ = *src++;
	    --size;
	}
    }
    *dst = '\0';
}

static struct tagFile {
    char *path;
    struct tagFile *next;
} *file_list = NULL;

static void add_to_filelist (char *path)
{
    struct tagFile *p;
    p = (struct tagFile *)malloc (sizeof (struct tagFile));
    p->path = strdup (path);
    p->next = file_list;
    file_list = p;
}

static int do_compile_files (int (__cdecl * PyRun_SimpleString)(char *),
			     int optimize)
{
    struct tagFile *p;
    int total, n;
    char Buffer[MAX_PATH + 64];
    int errors = 0;

    total = 0;
    p = file_list;
    while (p) {
	++total;
	p = p->next;
    }
    SendDlgItemMessage (hDialog, IDC_PROGRESS, PBM_SETRANGE, 0,
			MAKELPARAM (0, total));
    SendDlgItemMessage (hDialog, IDC_PROGRESS, PBM_SETPOS, 0, 0);

    n = 0;
    p = file_list;
    while (p) {
	++n;
        wsprintf (Buffer,
		  "import py_compile; py_compile.compile (r'%s')",
		  p->path);
        if (PyRun_SimpleString (Buffer)) {
	    ++errors;
	}
	/* We send the notification even if the files could not
	 * be created so that the uninstaller will remove them
	 * in case they are created later.
	 */
	wsprintf(Buffer, "%s%c", p->path, optimize ? 'o' : 'c');
	notify (FILE_CREATED, Buffer);

	SendDlgItemMessage (hDialog, IDC_PROGRESS, PBM_SETPOS, n, 0);
	SetDlgItemText (hDialog, IDC_INFO, p->path);
	p = p->next;
    }
    return errors;
}

/*
 * Returns number of files which failed to compile,
 * -1 if python could not be loaded at all
 */
static int compile_filelist (BOOL optimize_flag)
{
    void (__cdecl * Py_Initialize)(void);
    void (__cdecl * Py_SetProgramName)(char *);
    void (__cdecl * Py_Finalize)(void);
    int (__cdecl * PyRun_SimpleString)(char *);
    int *Py_OptimizeFlag;
    int errors = 0;
    HINSTANCE hPython;
    struct tagFile *p = file_list;

    if (!p)
	return 0;
    SetDlgItemText (hDialog, IDC_INFO, "Loading python...");
		
    hPython = LoadLibrary (pythondll);
    if (!hPython)
	return -1;
    Py_Initialize = (void (*)(void))GetProcAddress
	(hPython,"Py_Initialize");

    Py_SetProgramName = (void (*)(char *))GetProcAddress
	(hPython,"Py_SetProgramName");

    Py_Finalize = (void (*)(void))GetProcAddress (hPython,
						  "Py_Finalize");
    PyRun_SimpleString = (int (*)(char *))GetProcAddress (
	hPython, "PyRun_SimpleString");

    Py_OptimizeFlag = (int *)GetProcAddress (hPython,
					     "Py_OptimizeFlag");
    
    *Py_OptimizeFlag = optimize_flag;
    Py_SetProgramName(modulename);
    Py_Initialize ();

    errors += do_compile_files (PyRun_SimpleString, optimize_flag);
    Py_Finalize ();
    FreeLibrary (hPython);

    return errors;
}

static BOOL SystemError (int error, char *msg)
{
    char Buffer[1024];
    int n;

    if (error) {
        LPVOID lpMsgBuf;
	FormatMessage( 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	    FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL,
	    error,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    (LPSTR)&lpMsgBuf,
	    0,
	    NULL 
	    );
        strncpy (Buffer, lpMsgBuf, sizeof (Buffer));
	LocalFree (lpMsgBuf);
    } else
	Buffer[0] = '\0';
    n = lstrlen (Buffer);
    _snprintf (Buffer+n, sizeof (Buffer)-n, msg);
    MessageBox (hwndMain, Buffer, "Runtime Error", MB_OK | MB_ICONSTOP);
    return FALSE;
}

static BOOL AskOverwrite (char *filename)
{
    int result;
  again:
    if (allow_overwrite == ALWAYS)
	return TRUE;
    if (allow_overwrite == NEVER)
	return FALSE;
    if (allow_overwrite == ASK)
        return (IDYES == MessageBox (hwndMain,
			    filename,
			    "Overwrite existing file?",
			    MB_YESNO | MB_ICONQUESTION));

    result = MessageBox (hwndMain,
"Overwrite existing files?\n"
"\n"
"Press YES to ALWAYS overwrite existing files,\n"
"press NO to NEVER overwrite existing files,\n"
"press CANCEL to ASK individually.",
		         "Overwrite options",
			 MB_YESNOCANCEL | MB_ICONQUESTION);
    if (result == IDYES)
	allow_overwrite = ALWAYS;
    else if (result == IDNO)
	allow_overwrite = NEVER;
    else
	allow_overwrite = ASK;
    goto again;
}

static BOOL notify (int code, char *fmt, ...)
{
    char Buffer[1024];
    va_list marker;
    BOOL result = TRUE;
    int a, b;
    char *cp;

    va_start (marker, fmt);
    _vsnprintf (Buffer, sizeof (Buffer), fmt, marker);

    switch (code) {
/* Questions */
    case CAN_OVERWRITE:
	result = AskOverwrite (Buffer);
	break;

/* Information notification */
    case DIR_CREATED:
	if (logfile)
	    fprintf(logfile, "100 Made Dir: %s\n", fmt);
	break;

    case FILE_CREATED:
	if (logfile)
	    fprintf(logfile, "200 File Copy: %s\n", fmt);
	goto add_to_filelist_label;
	break;

    case FILE_OVERWRITTEN:
	if (logfile)
	    fprintf(logfile, "200 File Overwrite: %s\n", fmt);
      add_to_filelist_label:
	if ((cp = strrchr(fmt, '.')) && (0 == strcmp (cp, ".py")))
	     add_to_filelist (fmt);
	break;

/* Error Messages */
    case ZLIB_ERROR:
	MessageBox (GetFocus(), Buffer, "Error", MB_OK | MB_ICONWARNING);
	break;

    case SYSTEM_ERROR:
	SystemError (GetLastError(), Buffer);
	break;

    case NUM_FILES:
	a = va_arg (marker, int);
	b = va_arg (marker, int);
	SendMessage (hDialog, WM_NUMFILES, 0, MAKELPARAM (0, a));
	SendMessage (hDialog, WM_NEXTFILE, b, (LPARAM)fmt);
    }
    va_end (marker);
    
    return result;
}

static char *MapExistingFile (char *pathname, DWORD *psize)
{
    HANDLE hFile, hFileMapping;
    DWORD nSizeLow, nSizeHigh;
    char *data;

    hFile = CreateFile (pathname,
	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	return NULL;
    nSizeLow = GetFileSize (hFile, &nSizeHigh);
    hFileMapping = CreateFileMapping (hFile,
	NULL, PAGE_READONLY, 0, 0, NULL);
    CloseHandle (hFile);

    if (hFileMapping == INVALID_HANDLE_VALUE)
	return NULL;
    
    data = MapViewOfFile (hFileMapping,
	FILE_MAP_READ, 0, 0, 0);

    CloseHandle (hFileMapping);
    *psize = nSizeLow;
    return data;
}


static void create_bitmap(HWND hwnd)
{
    BITMAPFILEHEADER *bfh;
    BITMAPINFO *bi;
    HDC hdc;

    if (!bitmap_bytes)
	return;

    if (hBitmap)
	return;

    hdc = GetDC(hwnd);

    bfh = (BITMAPFILEHEADER *)bitmap_bytes;
    bi = (BITMAPINFO *)(bitmap_bytes + sizeof(BITMAPFILEHEADER));

    hBitmap = CreateDIBitmap(hdc,
			     &bi->bmiHeader,
			     CBM_INIT,
			     bitmap_bytes + bfh->bfOffBits,
			     bi,
			     DIB_RGB_COLORS);
    ReleaseDC(hwnd, hdc);
}

static char *ExtractIniFile (char *data, DWORD size, int *pexe_size)
{
    /* read the end of central directory record */
    struct eof_cdir *pe = (struct eof_cdir *)&data[size - sizeof
						  (struct eof_cdir)];
    
    int arc_start = size - sizeof (struct eof_cdir) - pe->nBytesCDir -
	pe->ofsCDir;

    int ofs = arc_start - sizeof (struct meta_data_hdr);

    /* read meta_data info */
    struct meta_data_hdr *pmd = (struct meta_data_hdr *)&data[ofs];
    char *src, *dst;
    char *ini_file;
    char tempdir[MAX_PATH];

    if (pe->tag != 0x06054b50) {
	return NULL;
    }

    if (pmd->tag != 0x1234567A || ofs < 0) {
	return NULL;
    }

    if (pmd->bitmap_size) {
	/* Store pointer to bitmap bytes */
	bitmap_bytes = (char *)pmd - pmd->uncomp_size - pmd->bitmap_size;
    }

    *pexe_size = ofs - pmd->uncomp_size - pmd->bitmap_size;

    src = ((char *)pmd) - pmd->uncomp_size;
    ini_file = malloc (MAX_PATH); /* will be returned, so do not free it */
    if (!ini_file)
	return NULL;
    if (!GetTempPath (sizeof (tempdir), tempdir)
	|| !GetTempFileName (tempdir, "~du", 0, ini_file)) {
	SystemError (GetLastError(), "Could not create temporary file");
	return NULL;
    }
    
    dst = map_new_file (CREATE_ALWAYS, ini_file, NULL, pmd->uncomp_size,
			0, 0, NULL/*notify*/);
    if (!dst)
	return NULL;
    memcpy (dst, src, pmd->uncomp_size);
    UnmapViewOfFile(dst);
    return ini_file;
}

static void PumpMessages (void)
{
    MSG msg;
    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
}

LRESULT CALLBACK
WindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HFONT hFont;
    int h;
    PAINTSTRUCT ps;
    switch (msg) {
    case WM_PAINT:
	hdc = BeginPaint (hwnd, &ps);
	h = GetSystemMetrics (SM_CYSCREEN) / 10;
	hFont = CreateFont (h, 0, 0, 0, 700, TRUE,
			    0, 0, 0, 0, 0, 0, 0, "Times Roman");
	hFont = SelectObject (hdc, hFont);
	SetBkMode (hdc, TRANSPARENT);
	TextOut (hdc, 15, 15, title, strlen (title));
	SetTextColor (hdc, RGB (255, 255, 255));
	TextOut (hdc, 10, 10, title, strlen (title));
	DeleteObject (SelectObject (hdc, hFont));
	EndPaint (hwnd, &ps);
	return 0;
    }
    return DefWindowProc (hwnd, msg, wParam, lParam);
}

static HWND CreateBackground (char *title)
{
    WNDCLASS wc;
    HWND hwnd;
    char buffer[4096];

    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hInstance = GetModuleHandle (NULL);
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush (RGB (0, 0, 128));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "SetupWindowClass";

    if (!RegisterClass (&wc))
	MessageBox (hwndMain,
		    "Could not register window class",
		    "Setup.exe", MB_OK);

    wsprintf (buffer, "Setup %s", title);
    hwnd = CreateWindow ("SetupWindowClass",
			 buffer,
			 0,
			 0, 0,
			 GetSystemMetrics (SM_CXFULLSCREEN),
			 GetSystemMetrics (SM_CYFULLSCREEN),
			 NULL,
			 NULL,
			 GetModuleHandle (NULL),
			 NULL);
    ShowWindow (hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow (hwnd);
    return hwnd;
}

/*
 * Center a window on the screen
 */
static void CenterWindow (HWND hwnd)
{
	RECT rc;
	int w, h;

	GetWindowRect (hwnd, &rc);
	w = GetSystemMetrics (SM_CXSCREEN);
	h = GetSystemMetrics (SM_CYSCREEN);
	MoveWindow (hwnd, (w - (rc.right-rc.left))/2, (h - (rc.bottom-rc.top))/2,
		rc.right-rc.left, rc.bottom-rc.top, FALSE);
}

#include <prsht.h>

BOOL CALLBACK
IntroDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnm;
    char Buffer[4096];

    switch (msg) {
    case WM_INITDIALOG:
	create_bitmap(hwnd);
	if (hBitmap)
	    SendDlgItemMessage(hwnd, IDC_BITMAP, STM_SETIMAGE,
			       IMAGE_BITMAP, (LPARAM)hBitmap);
	CenterWindow (GetParent (hwnd));
	wsprintf (Buffer,
		  "This Wizard will install %s on your computer. "
		  "Click Next to continue or Cancel to exit the Setup Wizard.",
		  meta_name);
	SetDlgItemText (hwnd, IDC_TITLE, Buffer);
	SetDlgItemText (hwnd, IDC_INTRO_TEXT, info);
	SetDlgItemText (hwnd, IDC_BUILD_INFO, build_info);
	return FALSE;

    case WM_NOTIFY:
        lpnm = (LPNMHDR) lParam;

        switch (lpnm->code) {
	case PSN_SETACTIVE:
	    PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT);
	    break;

	case PSN_WIZNEXT:
	    break;

	case PSN_RESET:
	    break;
		
	default:
	    break;
	}
    }
    return FALSE;
}

/*
 * Fill the listbox specified by hwnd with all python versions found
 * in the registry. version, if not NULL or empty, is the version
 * required.
 */
static BOOL GetPythonVersions (HWND hwnd, HKEY hkRoot, LPSTR version)
{
    DWORD index = 0;
    char core_version[80];
    HKEY hKey;
    BOOL result = TRUE;
    DWORD bufsize;

    if (ERROR_SUCCESS != RegOpenKeyEx (hkRoot,
				     "Software\\Python\\PythonCore",
				     0,	KEY_READ, &hKey))
	return FALSE;
    bufsize = sizeof (core_version);
    while (ERROR_SUCCESS == RegEnumKeyEx (hKey, index,
					  core_version, &bufsize, NULL,
					  NULL, NULL, NULL)) {
	char subkey_name[80], vers_name[80], prefix_buf[MAX_PATH+1];
	int itemindex;
	DWORD value_size;
	HKEY hk;

	bufsize = sizeof (core_version);
	++index;
	if (version && *version && strcmp (version, core_version))
		continue;

	wsprintf (vers_name, "Python Version %s (found in registry)",
		  core_version);
	itemindex = SendMessage (hwnd, LB_ADDSTRING, 0,
				 (LPARAM)(LPSTR)vers_name);
	wsprintf (subkey_name,
		  "Software\\Python\\PythonCore\\%s\\InstallPath",
		  core_version);
	value_size = sizeof (subkey_name);
	RegOpenKeyEx (hkRoot, subkey_name, 0, KEY_READ, &hk);
	RegQueryValueEx (hk, NULL, NULL, NULL, prefix_buf,
			 &value_size);
	RegCloseKey (hk);
	SendMessage (hwnd, LB_SETITEMDATA, itemindex,
		     (LPARAM)(LPSTR)strdup (prefix_buf));
    }
    RegCloseKey (hKey);
    return result;
}

BOOL CALLBACK
SelectPythonDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnm;

    switch (msg) {
    case WM_INITDIALOG:
	if (hBitmap)
	    SendDlgItemMessage(hwnd, IDC_BITMAP, STM_SETIMAGE,
			       IMAGE_BITMAP, (LPARAM)hBitmap);
	GetPythonVersions (GetDlgItem (hwnd, IDC_VERSIONS_LIST),
			   HKEY_LOCAL_MACHINE, target_version);
	GetPythonVersions (GetDlgItem (hwnd, IDC_VERSIONS_LIST),
			   HKEY_CURRENT_USER, target_version);
	{	/* select the last entry which is the highest python
		   version found */
	    int count;
	    count = SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST,
					LB_GETCOUNT, 0, 0);
	    if (count && count != LB_ERR)
		SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST, LB_SETCURSEL,
				    count-1, 0);
	}
	goto UpdateInstallDir;
	break;

    case WM_COMMAND:
	switch (LOWORD (wParam)) {
	case IDC_VERSIONS_LIST:
	    switch (HIWORD (wParam)) {
		int id;
		char *cp;
	    case LBN_SELCHANGE:
	      UpdateInstallDir:
		PropSheet_SetWizButtons(GetParent(hwnd),
					PSWIZB_BACK | PSWIZB_NEXT);
		id = SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST,
					 LB_GETCURSEL, 0, 0);
		if (id == LB_ERR) {
		    PropSheet_SetWizButtons(GetParent(hwnd),
					    PSWIZB_BACK);
		    SetDlgItemText (hwnd, IDC_PATH, "");
		    strcpy (install_dir, "");
		    strcpy (pythondll, "");
		} else {
		    char *pbuf;
		    int result;
		    PropSheet_SetWizButtons(GetParent(hwnd),
					    PSWIZB_BACK | PSWIZB_NEXT);
		    cp = (LPSTR)SendDlgItemMessage (hwnd,
						    IDC_VERSIONS_LIST,
						    LB_GETITEMDATA,
						    id,
						    0);
		    strcpy (install_dir, cp);
		    SetDlgItemText (hwnd, IDC_PATH, install_dir);
		    result = SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST,
					LB_GETTEXTLEN, (WPARAM)id, 0);
		    pbuf = (char *)malloc (result + 1);
		    if (pbuf) {
			/* guess the name of the python-dll */
			SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST,
					    LB_GETTEXT, (WPARAM)id,
					    (LPARAM)pbuf);
			result = sscanf (pbuf, "Python Version %d.%d",
					 &py_major, &py_minor);
			if (result == 2)
			    wsprintf (pythondll, "python%d%d.dll",
				      py_major, py_minor);
			free (pbuf);
		    } else
			strcpy (pythondll, "");
		}
	    }
	    break;
	}
	return 0;

    case WM_NOTIFY:
        lpnm = (LPNMHDR) lParam;

        switch (lpnm->code) {
	    int id;
	case PSN_SETACTIVE:
	    id = SendDlgItemMessage (hwnd, IDC_VERSIONS_LIST,
				     LB_GETCURSEL, 0, 0);
	    if (id == LB_ERR)
		PropSheet_SetWizButtons(GetParent(hwnd),
					PSWIZB_BACK);
	    else
		PropSheet_SetWizButtons(GetParent(hwnd),
					PSWIZB_BACK | PSWIZB_NEXT);
	    break;

	case PSN_WIZNEXT:
	    break;

	case PSN_WIZFINISH:
	    break;

	case PSN_RESET:
	    break;
		
	default:
	    break;
	}
    }
    return 0;
}

static void OpenLogfile(char *dir)
{
    char buffer[_MAX_PATH+1];
    time_t ltime;
    struct tm *now;
    long result;
    HKEY hKey, hSubkey;
    static char KeyName[] = 
	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    DWORD disposition;

    sprintf(buffer, "%s\\%s-wininst.log", dir, meta_name);
    logfile = fopen(buffer, "a");
    time (&ltime);
    now = localtime(&ltime);
    strftime(buffer, sizeof(buffer),
	     "*** Installation started %Y/%m/%d %H:%M ***\n",
	     localtime(&ltime));
    fprintf(logfile, buffer);
    fprintf(logfile, "Source: %s\n", modulename);

    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			  KeyName,
			  0,
			  KEY_ALL_ACCESS,
			  &hKey);

    if (result != ERROR_SUCCESS)
	MessageBox(GetFocus(), "Could not open key", KeyName, MB_OK);

    sprintf(buffer, "%s-py%d.%d", meta_name, py_major, py_minor);

    result = RegCreateKeyEx(hKey, buffer,
			    0, NULL, 0,
			    KEY_ALL_ACCESS,
			    NULL,
			    &hSubkey,
			    &disposition);

    if (result != ERROR_SUCCESS)
	MessageBox(GetFocus(), "Could not create key", buffer, MB_OK);

    RegCloseKey(hKey);

    if (disposition == REG_CREATED_NEW_KEY)
	fprintf(logfile, "020 Reg DB Key: [%s]%s\n", KeyName, buffer);

    sprintf(buffer, "Python %d.%d %s", py_major, py_minor, title);

    RegSetValueEx(hSubkey, "DisplayName",
		  0,
		  REG_SZ,
		  buffer,
		  strlen(buffer)+1);

    fprintf(logfile, "040 Reg DB Value: [%s]%s=%s\n",
	    KeyName, "DisplayName", buffer);

    {
	FILE *fp;
	sprintf(buffer, "%s\\Remove%s.exe", dir, meta_name);
	fp = fopen(buffer, "wb");
	fwrite(arc_data, exe_size, 1, fp);
	fclose(fp);

	sprintf(buffer, "\"%s\\Remove%s.exe\" -u \"%s\\%s-wininst.log\"",
		dir, meta_name, dir, meta_name);

	RegSetValueEx(hSubkey, "UninstallString",
		      0,
		      REG_SZ,
		      buffer,
		      strlen(buffer)+1);
	
	fprintf(logfile, "040 Reg DB Value: [%s]%s=%s\n",
		KeyName, "UninstallString", buffer);
    }
}

static void CloseLogfile(void)
{
    char buffer[_MAX_PATH+1];
    time_t ltime;
    struct tm *now;

    time (&ltime);
    now = localtime(&ltime);
    strftime(buffer, sizeof(buffer),
	     "*** Installation finished %Y/%m/%d %H:%M ***\n",
	     localtime(&ltime));
    fprintf(logfile, buffer);
    if (logfile)
	fclose(logfile);
}

BOOL CALLBACK
InstallFilesDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnm;
    char Buffer[4096];

    switch (msg) {
    case WM_INITDIALOG:
	if (hBitmap)
	    SendDlgItemMessage(hwnd, IDC_BITMAP, STM_SETIMAGE,
			       IMAGE_BITMAP, (LPARAM)hBitmap);
	wsprintf (Buffer,
		  "Click Next to begin the installation of %s. "
		  "If you want to review or change any of your "
		  " installation settings, click Back. "
		  "Click Cancel to exit the wizard.",
		  meta_name);
	SetDlgItemText (hwnd, IDC_TITLE, Buffer);
	break;

    case WM_NUMFILES:
	SendDlgItemMessage (hwnd, IDC_PROGRESS, PBM_SETRANGE, 0, lParam);
	PumpMessages ();
	return TRUE;

    case WM_NEXTFILE:
	SendDlgItemMessage (hwnd, IDC_PROGRESS, PBM_SETPOS, wParam,
			    0);
	SetDlgItemText (hwnd, IDC_INFO, (LPSTR)lParam);
	PumpMessages ();
	return TRUE;

    case WM_NOTIFY:
        lpnm = (LPNMHDR) lParam;

        switch (lpnm->code) {
	case PSN_SETACTIVE:
	    PropSheet_SetWizButtons(GetParent(hwnd),
				    PSWIZB_BACK | PSWIZB_NEXT);
	    break;

	case PSN_WIZFINISH:
	    break;

	case PSN_WIZNEXT:
	    /* Handle a Next button click here */
	    hDialog = hwnd;

	    /* Make sure the installation directory name ends in a */
	    /* backslash */
	    if (install_dir[strlen(install_dir)-1] != '\\')
		strcat (install_dir, "\\");
	    /* Strip the trailing backslash again */
	    install_dir[strlen(install_dir)-1] = '\0';
	    
	    OpenLogfile(install_dir);


	    /* Extract all files from the archive */
	    SetDlgItemText (hwnd, IDC_TITLE, "Installing files...");
	    success = unzip_archive (install_dir, arc_data,
				    arc_size, notify);
	    /* Compile the py-files */
	    if (pyc_compile) {
		int errors;
		SetDlgItemText (hwnd, IDC_TITLE,
				"Compiling files to .pyc...");
		errors = compile_filelist (FALSE);
		/* Compilation errors are intentionally ignored:
		 * Python2.0 contains a bug which will result
		 * in sys.path containing garbage under certain
		 * circumstances, and an error message will only
		 * confuse the user.
		 */
	    }
	    if (pyo_compile) {
		int errors;
		SetDlgItemText (hwnd, IDC_TITLE,
				"Compiling files to .pyo...");
		errors = compile_filelist (TRUE);
		/* Errors ignored: see above */
	    }

	    CloseLogfile();

	    break;

	case PSN_RESET:
	    break;
		
	default:
	    break;
	}
    }
    return 0;
}


BOOL CALLBACK
FinishedDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR lpnm;

    switch (msg) {
    case WM_INITDIALOG:
	if (hBitmap)
	    SendDlgItemMessage(hwnd, IDC_BITMAP, STM_SETIMAGE,
			       IMAGE_BITMAP, (LPARAM)hBitmap);
	if (!success)
	    SetDlgItemText (hwnd, IDC_INFO, "Installation failed.");
	return TRUE;

    case WM_NOTIFY:
        lpnm = (LPNMHDR) lParam;

        switch (lpnm->code) {
	case PSN_SETACTIVE: /* Enable the Finish button */
	    PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_FINISH);
	    break;

	case PSN_WIZNEXT:
	    break;

	case PSN_WIZFINISH:
	    break;

	case PSN_RESET:
	    break;
		
	default:
	    break;
	}
    }
    return 0;
}

void RunWizard (HWND hwnd)
{
    PROPSHEETPAGE   psp =       {0};
    HPROPSHEETPAGE  ahpsp[4] =  {0};
    PROPSHEETHEADER psh =       {0};

    /* Display module information */
    psp.dwSize =        sizeof(psp);
    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.hInstance =     GetModuleHandle (NULL);
    psp.lParam =        0;
    psp.pfnDlgProc =    IntroDlgProc;
    psp.pszTemplate =   MAKEINTRESOURCE(IDD_INTRO);

    ahpsp[0] =          CreatePropertySheetPage(&psp);

    /* Select python version to use */
    psp.dwFlags =       PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_SELECTPYTHON);
    psp.pfnDlgProc =        SelectPythonDlgProc;

    ahpsp[1] =              CreatePropertySheetPage(&psp);

    /* Install the files */
    psp.dwFlags =	    PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_INSTALLFILES);
    psp.pfnDlgProc =        InstallFilesDlgProc;

    ahpsp[2] =              CreatePropertySheetPage(&psp);

    /* Show success or failure */
    psp.dwFlags =           PSP_DEFAULT|PSP_HIDEHEADER;
    psp.pszTemplate =       MAKEINTRESOURCE(IDD_FINISHED);
    psp.pfnDlgProc =        FinishedDlgProc;

    ahpsp[3] =              CreatePropertySheetPage(&psp);

    /* Create the property sheet */
    psh.dwSize =            sizeof(psh);
    psh.hInstance =         GetModuleHandle (NULL);
    psh.hwndParent =        hwnd;
    psh.phpage =            ahpsp;
    psh.dwFlags =           PSH_WIZARD/*97*//*|PSH_WATERMARK|PSH_HEADER*/;
    psh.pszbmWatermark =    NULL;
    psh.pszbmHeader =       NULL;
    psh.nStartPage =        0;
    psh.nPages =            4;

    PropertySheet(&psh);
}

int DoInstall(void)
{
    char ini_buffer[4096];

    /* Read installation information */
    GetPrivateProfileString ("Setup", "title", "", ini_buffer,
			     sizeof (ini_buffer), ini_file);
    unescape (title, ini_buffer, sizeof(title));

    GetPrivateProfileString ("Setup", "info", "", ini_buffer,
			     sizeof (ini_buffer), ini_file);
    unescape (info, ini_buffer, sizeof(info));

    GetPrivateProfileString ("Setup", "build_info", "", build_info,
			     sizeof (build_info), ini_file);

    pyc_compile = GetPrivateProfileInt ("Setup", "target_compile", 1,
					ini_file);
    pyo_compile = GetPrivateProfileInt ("Setup", "target_optimize", 1,
					ini_file);

    GetPrivateProfileString ("Setup", "target_version", "",
			     target_version, sizeof (target_version),
			     ini_file);

    GetPrivateProfileString ("metadata", "name", "",
			     meta_name, sizeof (meta_name),
			     ini_file);

    hwndMain = CreateBackground (title);

    RunWizard (hwndMain);

    /* Clean up */
    UnmapViewOfFile (arc_data);
    if (ini_file)
	DeleteFile (ini_file);

    if (hBitmap)
	DeleteObject(hBitmap);

    return 0;
}

/*********************** uninstall section ******************************/

static int compare(const void *p1, const void *p2)
{
    return strcmp(*(char **)p2, *(char **)p1);
}

/*
 * Commit suicide (remove the uninstaller itself).
 *
 * Create a batch file to first remove the uninstaller
 * (will succeed after it has finished), then the batch file itself.
 *
 * This technique has been demonstrated by Jeff Richter,
 * MSJ 1/1996
 */
void remove_exe(void)
{
    char exename[_MAX_PATH];
    char batname[_MAX_PATH];
    FILE *fp;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    GetModuleFileName(NULL, exename, sizeof(exename));
    sprintf(batname, "%s.bat", exename);
    fp = fopen(batname, "w");
    fprintf(fp, ":Repeat\n");
    fprintf(fp, "del \"%s\"\n", exename);
    fprintf(fp, "if exist \"%s\" goto Repeat\n", exename);
    fprintf(fp, "del \"%s\"\n", batname);
    fclose(fp);
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    if (CreateProcess(NULL,
		      batname,
		      NULL,
		      NULL,
		      FALSE,
		      CREATE_SUSPENDED | IDLE_PRIORITY_CLASS,
		      NULL,
		      "\\",
		      &si,
		      &pi)) {
	SetThreadPriority(pi.hThread, THREAD_PRIORITY_IDLE);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	CloseHandle(pi.hProcess);
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
    }
}

void DeleteRegistryKey(char *line)
{
    char *keyname;
    char *subkeyname;
    char *delim;
    HKEY hKey;

    keyname = strchr(line, '[');
    if (!keyname)
	return;
    ++keyname;

    subkeyname = strchr(keyname, ']');
    if (!subkeyname)
	return;
    *subkeyname++='\0';
    delim = strchr(subkeyname, '\n');
    if (delim)
	*delim = '\0';

    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		 keyname,
		 0,
		 KEY_ALL_ACCESS,
		 &hKey);
    
    RegDeleteKey(hKey, subkeyname);
    RegCloseKey(hKey);
}

void DeleteRegistryValue(char *line)
{
    char *keyname;
    char *valuename;
    char *value;
    HKEY hKey;
/* Format is 'Reg DB Value: [key]name=value' */
    keyname = strchr(line, '[');
    if (!keyname)
	return;
    ++keyname;
    valuename = strchr(keyname, ']');
    if (!valuename)
	return;
    *valuename++ = '\0';
    value = strchr(valuename, '=');
    if (!value)
	return;

    *value++ = '\0';

    RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		 keyname,
		 0,
		 KEY_ALL_ACCESS,
		 &hKey);

    RegDeleteValue(hKey, valuename);
    RegCloseKey(hKey);
}

BOOL MyDeleteFile(char *line)
{
    char *pathname = strchr(line, ':');
    if (!pathname)
	return FALSE;
    ++pathname;
    while (isspace(*pathname))
	++pathname;
    return DeleteFile(pathname);
}

BOOL MyRemoveDirectory(char *line)
{
    char *pathname = strchr(line, ':');
    if (!pathname)
	return FALSE;
    ++pathname;
    while (isspace(*pathname))
	++pathname;
    return RemoveDirectory(pathname);
}

int DoUninstall(int argc, char **argv)
{
    FILE *logfile;
    char buffer[4096];
    int nLines = 0;
    int i;
    char *cp;
    int nFiles = 0;
    int nDirs = 0;
    int nErrors = 0;
    char **lines;
    int lines_buffer_size = 10;
    
    if (argc != 3) {
	MessageBox(NULL,
		   "Wrong number of args",
		   NULL,
		   MB_OK);
	return 1; /* Error */
    }
    if (strcmp(argv[1], "-u")) {
	MessageBox(NULL,
		   "2. arg is not -u",
		   NULL,
		   MB_OK);
	return 1; /* Error */
    }

    logfile = fopen(argv[2], "r");
    if (!logfile) {
	MessageBox(NULL,
		   "could not open logfile",
		   NULL,
		   MB_OK);
	return 1; /* Error */
    }
    
    lines = (char **)malloc(sizeof(char *) * lines_buffer_size);
    if (!lines)
	return SystemError(0, "Out of memory");

    /* Read the whole logfile, realloacting the buffer */
    while (fgets(buffer, sizeof(buffer), logfile)) {
	int len = strlen(buffer);
	/* remove trailing white space */
	while (isspace(buffer[len-1]))
	    len -= 1;
	buffer[len] = '\0';
	lines[nLines++] = strdup(buffer);
	if (nLines >= lines_buffer_size) {
	    lines_buffer_size += 10;
	    lines = (char **)realloc(lines,
				     sizeof(char *) * lines_buffer_size);
	    if (!lines)
		return SystemError(0, "Out of memory");
	}
    }
    fclose(logfile);

    /* Sort all the lines, so that highest 3-digit codes are first */
    qsort(&lines[0], nLines, sizeof(char *),
	  compare);

    if (IDYES != MessageBox(NULL,
			    "Are you sure you want to remove\n"
			    "this package from your computer?",
			    "Please confirm",
			    MB_YESNO | MB_ICONQUESTION))
	return 0;

    cp = "";
    for (i = 0; i < nLines; ++i) {
	/* Ignore duplicate lines */
	if (strcmp(cp, lines[i])) {
	    int ign;
	    cp = lines[i];
	    /* Parse the lines */
	    if (2 == sscanf(cp, "%d Made Dir: %s", &ign, &buffer)) {
		if (MyRemoveDirectory(cp))
		    ++nDirs;
		else {
		    int code = GetLastError();
		    if (code != 2 && code != 3) { /* file or path not found */
			++nErrors;
		    }
		}
	    } else if (2 == sscanf(cp, "%d File Copy: %s", &ign, &buffer)) {
		if (MyDeleteFile(cp))
		    ++nFiles;
		else {
		    int code = GetLastError();
		    if (code != 2 && code != 3) { /* file or path not found */
			++nErrors;
		    }
		}
	    } else if (2 == sscanf(cp, "%d File Overwrite: %s", &ign, &buffer)) {
		if (MyDeleteFile(cp))
		    ++nFiles;
		else {
		    int code = GetLastError();
		    if (code != 2 && code != 3) { /* file or path not found */
			++nErrors;
		    }
		}
	    } else if (2 == sscanf(cp, "%d Reg DB Key: %s", &ign, &buffer)) {
		DeleteRegistryKey(cp);
	    } else if (2 == sscanf(cp, "%d Reg DB Value: %s", &ign, &buffer)) {
		DeleteRegistryValue(cp);
	    }
	}
    }

    if (DeleteFile(argv[2])) {
	++nFiles;
    } else {
	++nErrors;
	SystemError(GetLastError(), argv[2]);
    }
    if (nErrors)
	wsprintf(buffer,
		 "%d files and %d directories removed\n"
		 "%d files or directories could not be removed",
		 nFiles, nDirs, nErrors);
    else
	wsprintf(buffer, "%d files and %d directories removed",
		 nFiles, nDirs);
    MessageBox(NULL, buffer, "Uninstall Finished!",
	       MB_OK | MB_ICONINFORMATION);
    remove_exe();
    return 0;
}

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
		    LPSTR lpszCmdLine, INT nCmdShow)
{
    extern int __argc;
    extern char **argv;
    char *basename;

    GetModuleFileName (NULL, modulename, sizeof (modulename));

    /* Map the executable file to memory */
    arc_data = MapExistingFile (modulename, &arc_size);
    if (!arc_data) {
	SystemError (GetLastError(), "Could not open archive");
	return 1;
    }

    /* OK. So this program can act as installer (self-extracting
     * zip-file, or as uninstaller when started with '-u logfile'
     * command line flags.
     *
     * The installer is usually started without command line flags,
     * and the uninstaller is usually started with the '-u logfile'
     * flag. What to do if some innocent user double-clicks the
     * exe-file?
     * The following implements a defensive strategy...
     */

    /* Try to extract the configuration data into a temporary file */
    ini_file = ExtractIniFile (arc_data, arc_size, &exe_size);

    if (ini_file)
	return DoInstall();

    if (!ini_file && __argc > 1) {
	return DoUninstall(__argc, __argv);
    }


    basename = strrchr(modulename, '\\');
    if (basename)
	++basename;

    /* Last guess about the purpose of this program */
    if (basename && (0 == strncmp(basename, "Remove", 6)))
	SystemError(0, "This program is normally started by windows");
    else
	SystemError(0, "Setup program invalid or damaged");
    return 1;
}
