#include <postgres.h>
#include <fmgr.h>
#include <utils/lsyscache.h>
#include <utils/array.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1( array_multi_index );
Datum array_multi_index( PG_FUNCTION_ARGS ) {
	ArrayType* values;
	ArrayType* indices;
	Oid values_type;
	int16 values_width;
	bool values_passbyvalue;
	char values_alignmentcode;
	Datum* values_content;
	bool* values_nullflags;
	int values_length;
	Oid indices_type;
	int16 indices_width;
	bool indices_passbyvalue;
	char indices_alignmentcode;
	Datum* indices_content;
	bool* indices_nullflags;
	int indices_length;
	Oid results_type;
	int16 results_width;
	bool results_passbyvalue;
	char results_alignmentcode;
	Datum* results_content;
	bool* results_nullflags;
	int results_length;
	int i;
	int dims[1];
	int lbs[1];
	ArrayType* results;
	if( PG_ARGISNULL(0) ) {
		PG_RETURN_NULL();
	}
	if( PG_ARGISNULL(1) ) {
		PG_RETURN_NULL();
	}
	values = PG_GETARG_ARRAYTYPE_P( 0 );
	indices = PG_GETARG_ARRAYTYPE_P( 1 );

	values_type = ARR_ELEMTYPE( values );
	get_typlenbyvalalign( values_type, &values_width, &values_passbyvalue, &values_alignmentcode );
	deconstruct_array( values, values_type, values_width, values_passbyvalue, values_alignmentcode, &values_content, &values_nullflags, &values_length );

	indices_type = ARR_ELEMTYPE( indices );
	get_typlenbyvalalign( indices_type, &indices_width, &indices_passbyvalue, &indices_alignmentcode );
	deconstruct_array( indices, indices_type, indices_width, indices_passbyvalue, indices_alignmentcode, &indices_content, &indices_nullflags, &indices_length );

	results_type = values_type;
	results_width = values_width;
	results_passbyvalue = values_passbyvalue;
	results_alignmentcode = values_alignmentcode;
	results_content = (Datum *)palloc( sizeof(Datum) * indices_length );
	results_nullflags = (bool *)palloc0( sizeof(bool) * indices_length );
	results_length = indices_length;

	for( i = 0; i < indices_length; ++i ) {
		if( indices_nullflags[i] ) {
			results_content[i] = 0;
			results_nullflags[i] = true;
		} else if( indices_content[i] - 1 >= (long unsigned)values_length ) {
			results_content[i] = 0;
			results_nullflags[i] = true;
		} else {
			results_content[i] = values_content[ indices_content[i] - 1 ];
			results_nullflags[i] = values_nullflags[ indices_content[i] - 1 ];
		}
	}

	dims[0] = results_length;
	lbs[0] = 1;
	results = construct_md_array( results_content, results_nullflags, 1, dims, lbs, results_type, results_width, results_passbyvalue, results_alignmentcode );
	pfree( results_content );
	pfree( results_nullflags );
	PG_RETURN_ARRAYTYPE_P( results );
}
