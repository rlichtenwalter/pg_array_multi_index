# pg_array_multi_index
simultaneous multiple-indexing of PostgreSQL arrays

<h3>Description</h3>
<p>PostgreSQL arrays allow organization of data into an arbitrary number of dimensions and come with sophisticated primitives to manage this complexity. Given an array <code>A</code> of <code>n</code> elements, indexing is achievable with <code>A[i]</code>, where <code>i</code> is a 1-indexed address in the array. Furthermore, it is possible to select slices of arrays a
s <code>A[i:j]</code>, where <code>i</code> and <code>j</code> specify the lower and upper bounds of the slice. PostgreSQL provides no efficient way to access multiple non-contiguous elements in its arrays. The standard way to access non-contiguous elements is as multiple fields in the <code>SELECT</code> clause (i.e. <code>SELECT A[i<sub>1</sub>], A[i<sub>2</sub>], A[i<sub>3</sub>], ...</code>). Each element selected in this manner causes the implementation to rescan the array. For non-contiguous subsets of indices approaching the length of the array, this results in O(n<sup>2</sup>) behavior.</p>

<p>Another method for selecting multiple elements is performing a set-returning <code>UNNEST WITH ORDINALITY</code> operation on the array. This provides the array elements with their associated indices. We can then select an arbitrary subset of the elements by joining and filtering the indices with other data. Finally, we can use the <code>array_agg</code> function to recompose the result into an array that contains only the elements to select. The entire procedure can be neatly wrapped in a SQL function, and it operates in O(n) time. Despite the superior asymptotic performance, this method is slow, and it is easily outperformed by selecting array elements as multiple fields when the number of fields to access is small.</p>

<p>It is possible to write a C function to overcome this difficulty. It accepts an array into which indexing should be performed and a second array of indices to use. We strove to maintain consistency with other PostgreSQL array primitives and allow as much generality as possible: the function returns SQL <code>NULL</code> for a <code>NULL</code> value array or index array, returns <code>NULL</code> array values corresponding to <code>NULL</code> or out-of-bounds indices, returns repeated values corresponding to repeated indices, and supports arbitrary index ordering. Since the underlying back-end implementation of PostgreSQL arrays is a C array, once the array is read from storage and unpacked, the only penalties involved in accessing elements are related to cache size and data locality. This function is therefore only slightly slower than native single-indexing via <code>A[i]</code>.</p>

<h3>Installation</h3>
<p>The following should be performed as root or at least in such a way that the performing user has sufficient privileges to perform the <code>make install</code> step.</p>

<pre>
curl -s -S -L https://github.com/rlichtenwalter/pg_array_multi_index/archive/master.zip &gt; pg_array_multi_index.zip
unzip pg_array_multi_index.zip
(cd pg_array_multi_index-master &amp;&amp; make PG_CONFIG=&lt;optional custom pg_config path&gt;)
(cd pg_array_multi_index-master &amp;&amp; make PG_CONFIG=&lt;optional custom pg_config path&gt; install)
(cd ~postgres &amp;&amp; sudo -u postgres psql -c 'CREATE EXTENSION pg_array_multi_index;')
</pre>

<h3>Usage</h3>
<pre>
testuser=# SELECT array_multi_index( ARRAY[0,1,2,3,4,5,6,7,8,9], ARRAY[1,2,3] );
 array_multi_index
-------------------
 {0,1,2}
(1 row)


testuser=# SELECT array_multi_index( ARRAY[0,NULL,2,3,4,5,6,7,8,9], ARRAY[1,2,3] );
 array_multi_index
\-------------------
 {0,NULL,2}
(1 row)

testuser=# SELECT array_multi_index( ARRAY[0,1,2,3,4,5,6,7,8,9], ARRAY[]::INTEGER[] );
 array_multi_index
\-------------------
 {}
(1 row)

testuser=# SELECT array_multi_index( ARRAY[0,1,2,3,4,5,6,7,8,9], ARRAY[3,5,7,7,7,9] );
 array_multi_index
\-------------------
 {2,4,6,6,6,8}
(1 row)

testuser=# SELECT array_multi_index( ARRAY[0,1,2,3,4,5,6,7,8,9], ARRAY[2,NULL,4] );
 array_multi_index
\-------------------
 {1,NULL,3}
(1 row)

testuser=# SELECT array_multi_index( ARRAY[0,1,2,3,4,5,6,7,8,9], NULL );
 array_multi_index
\-------------------

(1 row)

testuser=# SELECT array_multi_index( NULL::INTEGER[], ARRAY[2,4] );
 array_multi_index
\-------------------

(1 row)

testuser=# SELECT array_multi_index( ARRAY['I','work','with','any','array','type.'], ARRAY[1,2,3,4,5,6] );
       array_multi_index
\-------------------------------
 {I,work,with,any,array,type.}
(1 row)
</pre>
