/// @file
///
/// @brief Common interface for Integra serializers.
///
/// Copyright &copy; INTEGRA, Inc., 1999-2004.
///
/// @internal
/// @author Pnd - Nickolay Derjabin, '99.07.15

#ifndef _KLDB_SERIALIZER_HPP_
#define _KLDB_SERIALIZER_HPP_

#include "export.h"


INTEGRA_NAMESPACE_START
/// Type for array length. While it is int, but will be SIZE_T.
#define ARL_T int

class EntityType;
class PathStr;
class PlugRef;
class Str;
class UStr;

/// Abstract interface for data serializers.
class Serializer
  {
  public:
    /// @name High level serialization calls for basic types
    /// These are to process the whole chunk per call,
    /// and provide default values.
    //@{
    /// Serialize a boolean variable.
    INTAPI_BASE void Serialize(const char *tag,
                               bool     &var, bool     dval = 0);
    /// Serialize a char variable.
    INTAPI_BASE void Serialize(const char *tag,
                               char     &var, int      dval = 0);
    /// Serialize a short variable.
    INTAPI_BASE void Serialize(const char *tag,
                               short    &var, int      dval = 0);
    /// Serialize an integer variable.
    INTAPI_BASE void Serialize(const char *tag,
                               int      &var, int      dval = 0);
    /// Serialize an INT64 variable.
    INTAPI_BASE void Serialize(const char *tag,
                               INT64    &var, INT64    dval = 0);
    /// Serialize a BYTE variable.
    INTAPI_BASE void Serialize(const char *tag,
                               BYTE     &var, unsigned dval = 0);
    /// Serialize a WORD variable.
    INTAPI_BASE void Serialize(const char *tag,
                               WORD     &var, unsigned dval = 0);
    /// Serialize an unsigned variable.
    INTAPI_BASE void Serialize(const char *tag,
                               unsigned &var, unsigned dval = 0);
    /// Serialize a float variable.
    INTAPI_BASE void Serialize(const char *tag,
                               float    &var, double   dval = 0.0);
    /// Serialize a double variable.
    INTAPI_BASE void Serialize(const char *tag,
                               double   &var, double   dval = 0.0);
    /// Serialize pointer to EntityType.
    INTAPI_BASE void Serialize(const char *tag, EntityType *&var);
    //@}

  public:
    /// @name Auxiliary methods
    //@{
    /// Process obsolete chunks.
    INTAPI_BASE void Obsolete(const char *tag);
    /// Check for an expected data type.
    INTAPI_BASE virtual bool CheckValueType(int type);
    //@}

  public:
    /// @name Lower level serialization - common methods
    //@{
    /// Begin a complex sequence - a chunk.
    INTAPI_BASE virtual void BegChunk(const char *tag) = 0;
    /// End a complex sequence - a chunk.
    INTAPI_BASE virtual void EndChunk() = 0;
    /// Get serialization direction.
    inline bool Import();
    /// Get the number of chunk being serialized.
    INTAPI_BASE virtual int NChunk() = 0;
    /// Return a data version.
    inline virtual int DataVersion();
    //@}

    /// @name Input-ouput flags
    //@{
    /// INPUT/OUTPUT flags values.
    enum IO_FLAGS
      {
      /// The field is serialized for distributed calculations only.
      IO_FLAG_DISTRIBUTIVE = 0x1,
      /// To store the BSP tree.
      IO_FLAG_STORE_BSP = 0x2,
      /// RAW i-maps should be stored.
      IO_FLAG_RAW_IMAPS = 0x4,
      /// Filtered i-maps should be stored.
      IO_FLAG_FILTERED_IMAPS = 0x8,
      /// The field is serialized for CATDocument only.
      IO_FLAG_CATIA_DOCUMENT = 0x10,
      /// Flag pointing, that absolute paths should be removed during saving.
      IO_FLAG_SAVE_LOCAL = 0x20,
      /// Save vertex coordinates in Point3f instead of Obsolete chunk to provide forward compatibility. 
      /// Probably some other fields can be saved in the future for forward compatibility.
      IO_FLAG_FORWARD_COMPAT = 0x40,
      /// Store simulation results (i-maps, PT, BRT).
      IO_FLAG_STORE_SIM_RES = 0x80,
      /// Union of the flags responsible for storing i-maps the file.
      IO_FLAG_IMAPS = IO_FLAG_RAW_IMAPS | IO_FLAG_FILTERED_IMAPS,
      /// Union of the flags responsible for storing to the file.
      IO_FLAG_STORE_TO_FILE = IO_FLAG_STORE_BSP | IO_FLAG_IMAPS
      };
    //@}

    /// Access to the input-output flags.
    inline int &IOFlags();
    /// Get a serialization flag for distributed calculations.
    inline bool IsDistributive();
    /// Set a serialization flag for distributed calculations.
    //inline void SetDistributive();
    /// Get serialization flag for CATDocument.
    inline bool IsCATDocument();
    /// Set serialization flag for CATDocument.
    inline void SetCATDocument();
    /// Get serialization flag for Acceleration data storage.
    inline bool IsAccelDataStoring() const;
    /// Set serialization flag for Acceleration data storage.
    inline void SetAccelDataStoring(bool store);
    /// Get serialization flag for Simulation results storage.
    inline bool IsSimResStoring() const;
    /// Set serialization flag for Simulation results storage.
    inline void SetSimResStoring(bool store);

  public:
    /// @name Lower level serialization - variables of basic types
    //@{
    /// Serialize a boolean variable.
    INTAPI_BASE virtual void Value(bool     &var) = 0;
    /// Serialize a char variable.
    INTAPI_BASE virtual void Value(char     &var) = 0;
    /// Serialize a short variable.
    INTAPI_BASE virtual void Value(short    &var) = 0;
    /// Serialize an integer variable.
    INTAPI_BASE virtual void Value(int      &var) = 0;
    /// Serialize an INT64 variable.
    INTAPI_BASE virtual void Value(INT64    &var) = 0;
    /// Serialize a BYTE variable.
    INTAPI_BASE virtual void Value(BYTE     &var) = 0;
    /// Serialize a WORD variable.
    INTAPI_BASE virtual void Value(WORD     &var) = 0;
#if !defined(__linux) || defined(__x86_64)
    /// Serialize an unsigned variable.
    INTAPI_BASE virtual void Value(unsigned &var) = 0;
#endif
    /// Serialize an SIZE_T variable.
    INTAPI_BASE virtual void Value(SIZE_T   &var) = 0;
    /// Serialize a float variable.
    INTAPI_BASE virtual void Value(float    &var) = 0;
    /// Serialize a double variable.
    INTAPI_BASE virtual void Value(double   &var) = 0;
    /// Serialize pointer to EntityType.
    INTAPI_BASE virtual void Value(EntityType *&var) = 0;
    //@}

  public:
    /// @name Lower level serialization - arrays of basic types
    //@{
    /// Serialize an array of boolean.
    INTAPI_BASE virtual void Value(bool *arr, SIZE_T n) = 0;
    /// Serialize an array of char.
    INTAPI_BASE virtual void Value(char *arr, SIZE_T n) = 0;
    /// Serialize an array of short.
    INTAPI_BASE virtual void Value(short *arr, SIZE_T n) = 0;
    /// Serialize an array of integer.
    INTAPI_BASE virtual void Value(int *arr, SIZE_T n) = 0;
    /// Serialize an array of INT64.
    INTAPI_BASE virtual void Value(INT64 *arr, SIZE_T n) = 0;
    /// Serialize an array of BYTE.
    INTAPI_BASE virtual void Value(BYTE *arr, SIZE_T n) = 0;
    /// Serialize an array of WORD.
    INTAPI_BASE virtual void Value(WORD *arr, SIZE_T n) = 0;
    /// Serialize an array of unsigned.
    INTAPI_BASE virtual void Value(unsigned *arr, SIZE_T n) = 0;
    /// Serialize an array of float.
    INTAPI_BASE virtual void Value(float *arr, SIZE_T n) = 0;
    /// Serialize an array of double.
    INTAPI_BASE virtual void Value(double *arr, SIZE_T n) = 0;
    //@}
  public:
    /// @name Lower level serialization - strings
    //@{
    /// Serialize Str.
    INTAPI_BASE virtual void Value(Str     &var) = 0;
    /// Serialize UStr.
    INTAPI_BASE virtual void Value(UStr    &var) = 0;
    /// Serialize PathStr.
    INTAPI_BASE virtual void Value(PathStr &var) = 0;
    //@}

  public:
    /// @name Lower level serialization - references
    //@{
    /// Serialize a reference.
    INTAPI_BASE virtual void Value(PlugRef &var) = 0;
    //@}

  public:
    /// @name Error procession
    //@{
    /// Set an allocation error.
    INTAPI_BASE void SetAllocError();
    //@}

    /// Whether this is from/to a FILE or MEMORY
    INTAPI_BASE virtual bool IsFile() const = 0;


  protected:
    /// @name Error procession
    //@{
    /// Check an the allocation error.
    INTAPI_BASE bool AllocError();
    /// Clear an allocation error.
    INTAPI_BASE void ClearAllocError();
    /// Check for a format error.
    inline bool FormatError();
    /// Set a format error.
    INTAPI_BASE void SetFormatError();
    /// Clear a format error.
    INTAPI_BASE void ClearFormatError();
    //@}

  protected:
    /// @name Constructors, destructor
    //@{
    /// Constructor.
    INTAPI_BASE Serializer(bool imp);
    /// Copy constructor.
    inline Serializer(const Serializer &src);
    /// Destructor.
    inline virtual ~Serializer();
    //@}

    /// Ignore data for the current chunk.
    INTAPI_BASE virtual void IgnoreData() = 0;

  protected:
    /// Data version.
    int data_version;

  private:
    /// Direction: import (read) or export (write).
    bool import;

    /// @brief INPUT/OUTPUT flags.
    ///
    /// Distributive flag.
    /// true - if serialization for distributive calculations.
    /// It is set to false by class constructor and must be set to true by
    /// the Serialization method of the first which start such serialization.
    /// Each class should serialize this check if its serialization
    /// depends on it.
    ///
    /// Acceleration data flag.
    /// true - for storing Ray Tracer acceleration data - BSP tree, that
    /// allows to not create it once more during second and other sessions.
    int io_flags;

    /// Format error flag.
    bool format_error;
    /// Allocation error flag.
    bool alloc_error;
  };  // class Serializer


//////////////////////////////////////////////////////////////////////////////
// Inline methods of Serializer.

//////////////////////////////////////////////////////////////////////////////
/// Copy constructor.
/// @note The method is forbidden, debug version asserts it.
/// @param[in] src - A source object.
Serializer::Serializer(const Serializer &src)
  {
  Assert(false);
  }

//////////////////////////////////////////////////////////////////////////////
/// Destructor.
///
/// The method destroys the serializer.
Serializer::~Serializer()
  {
  }

//////////////////////////////////////////////////////////////////////////////
/// Get serialization direction.
///
/// The method informs about serialization direction (import or export).
/// @return YES, if this serializer imports (reads) data,
/// and NO, if this serializers exports (writes) data.
bool Serializer::Import()
  {
  return import;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get serialization flag for distributive calculations.
/// @return YES, if serialization is distributive; NO otherwise.
bool Serializer::IsDistributive()
  {
  return ((io_flags & IO_FLAG_DISTRIBUTIVE) == IO_FLAG_DISTRIBUTIVE);
  }

#if 0 // Only  RPC can set this flag.
//////////////////////////////////////////////////////////////////////////////
/// Set serialization flag for distributive calculations.
void Serializer::SetDistributive()
  {
  io_flags |= IO_FLAG_DISTRIBUTIVE;
  }
#endif

//////////////////////////////////////////////////////////////////////////////
/// Get serialization flag for CATDocument.
/// @return YES, if serialization is for CATDocument; NO otherwise.
bool Serializer::IsCATDocument()
  {
  return ((io_flags & IO_FLAG_CATIA_DOCUMENT) == IO_FLAG_CATIA_DOCUMENT);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set serialization flag for CATDocument.
void Serializer::SetCATDocument()
  {
  io_flags |= IO_FLAG_CATIA_DOCUMENT;
  }

//////////////////////////////////////////////////////////////////////////////
/// Get serialization flag for Acceleration data storage.
/// @return true, if acceleration data must be stored, false - otherwise
bool Serializer::IsAccelDataStoring() const
  {
  return ((io_flags & IO_FLAG_STORE_BSP) == IO_FLAG_STORE_BSP);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set serialization flag for Acceleration data storage.
/// @param[in] store Store acceleration data.
void Serializer::SetAccelDataStoring(bool store)
  {
  if (store)
    io_flags |= IO_FLAG_STORE_BSP;
  else
    io_flags &= ~(IO_FLAG_STORE_BSP);
  }

//////////////////////////////////////////////////////////////////////////////
/// Get serialization flag for Simulation results storage.
/// @return true, if Simulation results must be stored, false - otherwise
bool Serializer::IsSimResStoring() const
  {
  return ((io_flags & IO_FLAG_STORE_SIM_RES) == IO_FLAG_STORE_SIM_RES);
  }

//////////////////////////////////////////////////////////////////////////////
/// Set serialization flag for Simulation results storage.
/// @param[in] store Store Simulation results.
void Serializer::SetSimResStoring(bool store)
  {
  if (store)
    io_flags |= IO_FLAG_STORE_SIM_RES;
  else
    io_flags &= ~(IO_FLAG_STORE_SIM_RES);
  }

//////////////////////////////////////////////////////////////////////////////
/// Access to the input-output flags.
/// @return Reference to the input-output flags.
int &Serializer::IOFlags()
  {
  return io_flags;
  }

//////////////////////////////////////////////////////////////////////////////
/// Check for format error.
///
/// The method checks if an error in serialized data was found while serialization.
/// @return YES if a format error was found during serialization, NO otherwise.
bool Serializer::FormatError()
  {
  return format_error;  // bad data
  }

//////////////////////////////////////////////////////////////////////////////
/// Returns a data version.
/// @return A version of the data, corresponding to the version of Repository
/// (for example, 0x032100 for Repository 3.21).
/// This version data will be written into an IOF file as the very first field
/// in the binary IOF data and it will be read from new IOF files.
int Serializer::DataVersion()
  {
  return data_version;
  }

INTEGRA_NAMESPACE_END
#endif
