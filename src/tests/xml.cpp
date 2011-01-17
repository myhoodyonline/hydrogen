
#include <hydrogen/helpers/filesystem.h>

#include <hydrogen/basics/drumkit.h>
#include <hydrogen/basics/instrument.h>
#include <hydrogen/basics/instrument_list.h>
#include <hydrogen/basics/instrument_layer.h>
#include <hydrogen/basics/sample.h>

#define BASE_DIR    "./src/tests/data"

static void spec( bool cond, const char* msg ) {
    if( !cond ) {
        ___ERRORLOG( QString(" ** SPEC : %1" ).arg( msg ) );
        sleep( 1 );
        exit( EXIT_FAILURE);
    }
}

static bool check_samples_data( H2Core::Drumkit* dk, bool loaded ) {
    H2Core::InstrumentList* instruments = dk->get_instruments();
    for( int i=0; i<instruments->size(); i++ ) {
        H2Core::Instrument* instrument = ( *instruments )[i];
        for ( int n = 0; n < MAX_LAYERS; n++ ) {
            H2Core::InstrumentLayer* layer = instrument->get_layer( n );
            if( layer ) {
                H2Core::Sample* sample = layer->get_sample();
                if( loaded ) {
                    if( sample->get_data_l()==0 || sample->get_data_l()==0 ) return false;
                } else {
                    if( sample->get_data_l()!=0 || sample->get_data_l()!=0 ) return false;
                }
            }
        }
    }
    return true;
}

int xml_drumkit( int log_level ) {

    H2Core::Filesystem::rm( BASE_DIR"/dk0", true );
    H2Core::Filesystem::rm( BASE_DIR"/drumkit.xml" );

    ___INFOLOG( "test xml drumkit validation, read and write" );

    H2Core::Drumkit* dk0 = 0;
    H2Core::Drumkit* dk1 = 0;
    H2Core::Drumkit* dk2 = 0;

    // load without samples
    dk0 = H2Core::Drumkit::load( BASE_DIR"/drumkit" );
    spec( dk0!=0, "dk0 should not be null" );
    spec( dk0->samples_loaded()==false, "samples should NOT be loaded" );
    spec( check_samples_data( dk0, false ), "sample data should be NULL" );
    //dk0->dump();
    // manually load samples
    spec( dk0->load_samples()==true, "should be able to load sample" );
    spec( dk0->samples_loaded()==true, "samples should be loaded" );
    spec( check_samples_data( dk0, true ), "sample data should NOT be NULL" );
    //dk0->dump();
    // load with samples
    dk0 = H2Core::Drumkit::load( BASE_DIR"/drumkit", true );
    spec( dk0!=0, "dk0 should not be null" );
    spec( dk0->samples_loaded()==true, "samples should be loaded" );
    spec( check_samples_data( dk0, true ), "sample data should NOT be NULL" );
    //dk0->dump();
    // unload samples
    spec( dk0->unload_samples(), "should be able to unload samples" );
    spec( dk0->samples_loaded()==false, "samples should NOT be loaded" );
    spec( check_samples_data( dk0, false ), "sample data should be NULL" );
    //dk0->dump();
    // save drumkit elsewhere
    dk0->set_name( "dk0" );
    spec( dk0->save( BASE_DIR"/dk0", false ), "should be able to save drumkit" );
    spec( H2Core::Filesystem::file_readable( BASE_DIR"/dk0/drumkit.xml"), "dk0/drumkit.xml should exists and be readable" );
    spec( H2Core::Filesystem::file_readable( BASE_DIR"/dk0/crash.wav"), "dk0/crash.wav should exists and be readable" );
    spec( H2Core::Filesystem::file_readable( BASE_DIR"/dk0/hh.wav"), "dk0/hh.wav should exists and be readable" );
    spec( H2Core::Filesystem::file_readable( BASE_DIR"/dk0/kick.wav"), "dk0/kick.wav should exists and be readable" );
    spec( H2Core::Filesystem::file_readable( BASE_DIR"/dk0/snare.wav"), "dk0/snare.wav should exists and be readable" );
    // load file
    dk1 = H2Core::Drumkit::load_file( BASE_DIR"/dk0/drumkit.xml" );
    spec( dk1!=0, "should be able to reload drumkit" );
    //dk1->dump();
    // copy constructor
    dk2 = new H2Core::Drumkit( dk1 );
    dk2->set_name("COPY");
    spec( dk2!=0, "should be able to copy a drumkit" );
    // save file
    spec( dk2->save_file( BASE_DIR"/drumkit.xml", true ), "should be able to save drumkit xml file" );;

    delete dk0;
    delete dk1;
    delete dk2;
    
    return EXIT_SUCCESS;
}
