#include "platform.h"

extern int udpxy_main( int argc, char* const argv[] );

#ifdef UDPXREC_MOD
extern int udpxrec_main( int argc, char* const argv[] );
#endif

int
main( int argc, char* const argv[] )
{
#if defined(_WIN32)
    if (argc > 1 && strcmp(argv[1], "rec") == 0) {
        return udpxrec_main(argc - 1, argv + 1);
    }
    return udpxy_main(argc, argv);
#else
    const char* app = basename(argv[0]);

    if( 0 == strncmp( "udpxy", app, sizeof("udpxy") ) )
        return udpxy_main( argc, argv );
#ifdef UDPXREC_MOD
    else if( 0 == strncmp( "udpxrec", app, sizeof("udpxrec") ) )
        return udpxrec_main( argc, argv );
#endif

    (void)fprintf( stderr, "Unsupported udpxy module [%s]\n", app);
    return 1;
#endif
}
