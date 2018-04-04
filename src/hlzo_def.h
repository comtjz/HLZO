//
// Created by hongchao1 on 2018/3/30.
//

#ifndef HLZO_HLZO_DEF_H
#define HLZO_HLZO_DEF_H

#define ADLER32_INIT_VALUE  1
#define CRC32_INIT_VALUE    0

#define LZOP_VERSION            0x1030
#define LZOP_VERSION_STRING     "1.03"
#define LZOP_VERSION_DATE       "Nov 1st 2010"

#ifndef SUFFIX_MAX
#define SUFFIX_MAX   32
#endif

#define STDIN_NAME      "<stdin>"
#define STDOUT_NAME     "<stdout>"
#define STDERR_NAME     "<stderr>"
#define UNKNOWN_NAME    "<unknown>"

//length of one line in config file and path
#define HLZO_LINE_MAX_LEN			     1024

// Return code
#define HLZO_OK                          0
#define HLZO_ERR_PARAM                   1

// error of file
#define HLZO_ERR_OPEN                    51

#define ALIGN_SIZE    4096

#define BLOCK_SIZE        (256*1024l)
const u_int64_t block_size = BLOCK_SIZE;

#define MAX_BLOCK_SIZE      (64*1024l*1024l)        /* DO NOT CHANGE */

/* LZO may expand uncompressible data by a small amount */
#define MAX_COMPRESSED_SIZE(x)  ((x) + (x) / 16 + 64 + 3)


/*************************************************************************
// lzop file header
**************************************************************************/

/* header flags */
#define F_ADLER32_D     0x00000001L
#define F_ADLER32_C     0x00000002L
#define F_STDIN         0x00000004L
#define F_STDOUT        0x00000008L
#define F_NAME_DEFAULT  0x00000010L
#define F_DOSISH        0x00000020L
#define F_H_EXTRA_FIELD 0x00000040L
#define F_H_GMTDIFF     0x00000080L
#define F_CRC32_D       0x00000100L
#define F_CRC32_C       0x00000200L
#define F_MULTIPART     0x00000400L
#define F_H_FILTER      0x00000800L
#define F_H_CRC32       0x00001000L
#define F_H_PATH        0x00002000L
#define F_MASK          0x00003FFFL

/* operating system & file system that created the file [mostly unused] */
#define F_OS_FAT        0x00000000L         /* DOS, OS2, Win95 */
#define F_OS_AMIGA      0x01000000L
#define F_OS_VMS        0x02000000L
#define F_OS_UNIX       0x03000000L
#define F_OS_VM_CMS     0x04000000L
#define F_OS_ATARI      0x05000000L
#define F_OS_OS2        0x06000000L         /* OS2 */
#define F_OS_MAC9       0x07000000L
#define F_OS_Z_SYSTEM   0x08000000L
#define F_OS_CPM        0x09000000L
#define F_OS_TOPS20     0x0a000000L
#define F_OS_NTFS       0x0b000000L         /* Win NT/2000/XP */
#define F_OS_QDOS       0x0c000000L
#define F_OS_ACORN      0x0d000000L
#define F_OS_VFAT       0x0e000000L         /* Win32 */
#define F_OS_MFS        0x0f000000L
#define F_OS_BEOS       0x10000000L
#define F_OS_TANDEM     0x11000000L
#define F_OS_SHIFT      24
#define F_OS_MASK       0xff000000L

/* character set for file name encoding [mostly unused] */
#define F_CS_NATIVE     0x00000000L
#define F_CS_LATIN1     0x00100000L
#define F_CS_DOS        0x00200000L
#define F_CS_WIN32      0x00300000L
#define F_CS_WIN16      0x00400000L
#define F_CS_UTF8       0x00500000L         /* filename is UTF-8 encoded */
#define F_CS_SHIFT      20
#define F_CS_MASK       0x00f00000L

/* these bits must be zero */
#define F_RESERVED      ((F_MASK | F_OS_MASK | F_CS_MASK) ^ 0xffffffffL)

const unsigned char lzop_magic[9] = {0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a};

enum {
    M_LZO1X_1     =     1,
    M_LZO1X_1_15  =     2,
    M_LZO1X_999   =     3,
    M_NRV1A       =  0x1a,
    M_NRV1B       =  0x1b,
    M_NRV2A       =  0x2a,
    M_NRV2B       =  0x2b,
    M_NRV2D       =  0x2d,
    M_ZLIB        =   128,

    M_UNUSED
};

#endif //HLZO_HLZO_DEF_H
