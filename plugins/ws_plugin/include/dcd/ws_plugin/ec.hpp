#ifndef EC_HPP
#define EC_HPP
/*                                                                
                              ;;;;;;: ,++++++++++++++++++          
                             .;;;;;;  ++++++++++++++++++           
                             ;;;;;;. '+++++++++++++++++'           
                            :;;;;;;  ++++++++++++++++++            
                            ;;;;;;  ++++++++++++++++++,            
                           ;;;;;;: `++++++::::::::::::             
                          `;;;;;;  ++++++:                         
                          ;;;;;;, :++++++                          
                         ,;;;;;;  ++++++`                          
             ;;;;;;;;;;;;;;;;;;` '++++++                           
            .;;;;;;;;;;;;;;;;;;  ++++++                            
            ;;;;;;;;;;;;;;;;;;  ++++++'                            
           :;;;;;;;;;;;;;;;;., `++++++                             
           ;;;;;;;;;;;;;;;.    ++++++,                             
          ;;;;;;;;;;;;;;;;      +++++                              
                                ++++`                              
          ,,,,,,,,,,,,,,,,      +++++                              
          ,+++++++++++++++     `+++++                              
           ++++++++++++++++    '++++++                             
           `++++++++++++++++++  ++++++,                            
            ++++++++++++++++++; ,++++++                            
             ++++++++++++++++++  ++++++'                           
                         +++++++ `++++++                           
                          ++++++  +++++++                          
                          '++++++  ++++++`                         
                           ++++++, ;++++++                         
                           ,++++++  ++++++                         
                            ++++++' .++++                          
                             ++++++  +++:                          
                             +++++++  ++                           
                              ++++++` +`                           
                              ;++++++                             
                                                           
*/

#include <secp256k1.h>
#include <openssl/sha.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <memory>

 #include <openssl/ripemd.h>


#include "debug_levels.hpp"

static int extended_nonce_function( unsigned char *nonce32, const unsigned char *msg32,
                                        const unsigned char *key32, unsigned int attempt,
                                        const void *data ) {
    unsigned int* extra = (unsigned int*) data;
    (*extra)++;
    return secp256k1_nonce_function_default( nonce32, msg32, key32, *extra, nullptr );
}

inline std::vector<uint8_t> ripemd160( const std::vector<uint8_t>& data ) {
    std::vector<uint8_t> hash;
    hash.resize(RIPEMD160_DIGEST_LENGTH);
    RIPEMD160_CTX ripemd160;
    RIPEMD160_Init(&ripemd160);
    RIPEMD160_Update(&ripemd160, data.data(), data.size());
    RIPEMD160_Final(hash.data(), &ripemd160);
    return hash;
};

inline std::vector<uint8_t> hex_string_to_byte_array(const std::string& hex) {
  std::vector<uint8_t> bytes;
  for( uint32_t i = 0; i < hex.length( ); i += 2 ){
    std::string byteString = hex.substr( i, 2 );
    uint8_t byte = ( uint8_t )strtol( byteString.c_str( ), NULL, 16 );
    bytes.push_back( byte );
  }
  return bytes;
}

inline std::string byte_array_to_hex_string( const uint8_t* data, const uint32_t& size ) {
    std::stringstream ss;
    ss << std::hex;
    for( uint32_t i = 0; i < size; i++ )
        ss << std::setw( 2 ) << std::setfill( '0' ) << ( int )data[i];
    return ss.str( );    
}

inline std::string byte_array_to_hex_string( const std::vector<uint8_t> data ) {
    std::stringstream ss;
    ss << std::hex;
    for( uint32_t i = 0; i < data.size( ); i++ )
        ss << std::setw( 2 ) << std::setfill( '0' ) << ( int )data[i];
    return ss.str( );    
}

class ec_rnd_uint8_t_generator{
protected:
    std::mt19937 _mt;
    std::uniform_int_distribution<> _distribution;

public:
    ec_rnd_uint8_t_generator( ) : _mt( std::random_device( )( ) ), _distribution( std::uniform_int_distribution<>( 0, 255 ) ) {
    }

    uint8_t get_rnd_byte( ){
        return _distribution( _mt );
    }
};

class ec_rnd_uint32_t_generator{
protected:
    std::mt19937 _mt;
    std::uniform_int_distribution<> _distribution;

public:
    ec_rnd_uint32_t_generator( ) : _mt( std::random_device( )( ) ), _distribution( std::uniform_int_distribution<>( 0, 2147483647 ) ) {
    }

    uint32_t get_rnd_byte( ){
        return _distribution( _mt );
    }
};

class sha256_impl{
public:
    sha256_impl( ){
        SHA256_Init( &_ctx );

        ec_rnd_uint8_t_generator g;
        std::vector<uint8_t> v( 256 );
        for( uint32_t i = 0; i < 256; i++ )
            v[i] = g.get_rnd_byte( );

        SHA256_Update( &_ctx, &v[0], v.size( ) ); 
        SHA256_Final( key, &_ctx );      
    }

    sha256_impl( const std::string& data ){
        SHA256_Init( &_ctx );

        std::vector<uint8_t> v( data.begin( ), data.end( ) );

        SHA256_Update( &_ctx, &v[0], v.size( ) ); 
        SHA256_Final( key, &_ctx );  
    }

    sha256_impl( const std::vector<uint8_t>& v ){
        SHA256_Init( &_ctx );
        SHA256_Update( &_ctx, &v[0], v.size( ) ); 
        SHA256_Final( key, &_ctx );  
    }

    sha256_impl( const uint8_t* data, const uint32_t& data_size ){
        SHA256_Init( &_ctx );
        SHA256_Update( &_ctx, data, data_size ); 
        SHA256_Final( key, &_ctx );  
    }

    sha256_impl( const std::shared_ptr<std::vector<uint8_t>>& v ){
        SHA256_Init( &_ctx );
        SHA256_Update( &_ctx, &v.get( )->at( 0 ), v.get( )->size( ) ); 
        SHA256_Final( key, &_ctx );  
    }

    ~sha256_impl( ){
    }

    std::vector<uint8_t> to_byte_array( ) const{
        return std::vector<uint8_t>( key, key + SHA256_DIGEST_LENGTH );
    }

    std::string to_hex_string( ) const{
        std::stringstream ss;
        ss << std::hex;
        for( int i = 0; i < SHA256_DIGEST_LENGTH; i++ )
            ss << std::setw( 2 ) << std::setfill( '0' ) << ( int )key[i];
        return ss.str( );
    }

    const uint8_t* get_hash( ) const{
        return key;
    }

    uint8_t* get_data( ){
        return key;
    }

    std::vector<uint8_t> get_hash( ){
        return std::vector<uint8_t>(key, key+SHA256_DIGEST_LENGTH);
    }

private:
    SHA256_CTX _ctx;
public:    
    uint8_t key[SHA256_DIGEST_LENGTH];
};

class secp256k1_impl{
public:
    secp256k1_impl( ) : _ctx( NULL ){
        _ctx = secp256k1_context_create( SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_RANGEPROOF | SECP256K1_CONTEXT_COMMIT );
        if( !_ctx )
            throw std::runtime_error( " can't secp256k1_context_create ctx" );                
    };

    secp256k1_impl( const std::string& seed ){
        std::vector<uint8_t> v( seed.begin( ), seed.end( ) );

        _ctx = secp256k1_context_create( SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_RANGEPROOF | SECP256K1_CONTEXT_COMMIT );
        if( !_ctx )
            throw std::runtime_error( " can't secp256k1_context_create ctx" );

        if( !secp256k1_context_randomize( _ctx, &v[0] ) )
            throw std::runtime_error( " can't secp256k1_context_randomize ctx" );  
    }

    secp256k1_impl( const std::vector<uint8_t>& seed ){
        _ctx = secp256k1_context_create( SECP256K1_CONTEXT_VERIFY | SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_RANGEPROOF | SECP256K1_CONTEXT_COMMIT);
        if( !_ctx )
            throw std::runtime_error( " can't secp256k1_context_create ctx" );

        if( !secp256k1_context_randomize( _ctx, &seed[0] ) )
            throw std::runtime_error( " can't secp256k1_context_randomize ctx" );  
    }

    secp256k1_impl( secp256k1_context_t* ctx ){
        _ctx = ctx; 
    }

    secp256k1_impl( const secp256k1_impl& ctx ){
        _ctx = ctx._ctx; 
    }

    bool is_canonical( const std::vector<uint8_t>& c ) {
        return !(c[1] & 0x80)
               && !(c[1] == 0 && !(c[2] & 0x80))
               && !(c[33] & 0x80)
               && !(c[33] == 0 && !(c[34] & 0x80));
    }

    std::vector<uint8_t> sign_data_compact( const std::vector<uint8_t>& private_key, const std::vector<uint8_t>& hash_for_data ) {
        std::vector<uint8_t> signature;
        signature.resize(65);
        int recid;
        unsigned int counter = 0;

        do {
            secp256k1_ecdsa_sign_compact( _ctx, hash_for_data.data(), &signature[1], &private_key[0], extended_nonce_function, &counter, &recid );
        } while (!is_canonical(signature));
       
        signature.begin()[0] = 27 + 4 + recid;
        return signature;                      
    }
    
    ~secp256k1_impl( ){
        clear( );
    };

    secp256k1_context_t* clone( ) {
        return secp256k1_context_clone( _ctx );
    };

    void clear( ){
        secp256k1_context_destroy( _ctx );
    }
private:
    secp256k1_context_t* _ctx;    
};

#endif
