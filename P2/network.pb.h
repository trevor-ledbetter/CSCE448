// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: network.proto

#ifndef PROTOBUF_network_2eproto__INCLUDED
#define PROTOBUF_network_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_network_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[6];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsFollowRequestImpl();
void InitDefaultsFollowRequest();
void InitDefaultsFollowReplyImpl();
void InitDefaultsFollowReply();
void InitDefaultsUnfollowRequestImpl();
void InitDefaultsUnfollowRequest();
void InitDefaultsUnfollowReplyImpl();
void InitDefaultsUnfollowReply();
void InitDefaultsListRequestImpl();
void InitDefaultsListRequest();
void InitDefaultsListReplyImpl();
void InitDefaultsListReply();
inline void InitDefaults() {
  InitDefaultsFollowRequest();
  InitDefaultsFollowReply();
  InitDefaultsUnfollowRequest();
  InitDefaultsUnfollowReply();
  InitDefaultsListRequest();
  InitDefaultsListReply();
}
}  // namespace protobuf_network_2eproto
namespace network {
class FollowReply;
class FollowReplyDefaultTypeInternal;
extern FollowReplyDefaultTypeInternal _FollowReply_default_instance_;
class FollowRequest;
class FollowRequestDefaultTypeInternal;
extern FollowRequestDefaultTypeInternal _FollowRequest_default_instance_;
class ListReply;
class ListReplyDefaultTypeInternal;
extern ListReplyDefaultTypeInternal _ListReply_default_instance_;
class ListRequest;
class ListRequestDefaultTypeInternal;
extern ListRequestDefaultTypeInternal _ListRequest_default_instance_;
class UnfollowReply;
class UnfollowReplyDefaultTypeInternal;
extern UnfollowReplyDefaultTypeInternal _UnfollowReply_default_instance_;
class UnfollowRequest;
class UnfollowRequestDefaultTypeInternal;
extern UnfollowRequestDefaultTypeInternal _UnfollowRequest_default_instance_;
}  // namespace network
namespace network {

// ===================================================================

class FollowRequest : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.FollowRequest) */ {
 public:
  FollowRequest();
  virtual ~FollowRequest();

  FollowRequest(const FollowRequest& from);

  inline FollowRequest& operator=(const FollowRequest& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  FollowRequest(FollowRequest&& from) noexcept
    : FollowRequest() {
    *this = ::std::move(from);
  }

  inline FollowRequest& operator=(FollowRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const FollowRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const FollowRequest* internal_default_instance() {
    return reinterpret_cast<const FollowRequest*>(
               &_FollowRequest_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(FollowRequest* other);
  friend void swap(FollowRequest& a, FollowRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline FollowRequest* New() const PROTOBUF_FINAL { return New(NULL); }

  FollowRequest* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const FollowRequest& from);
  void MergeFrom(const FollowRequest& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(FollowRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // @@protoc_insertion_point(class_scope:network.FollowRequest)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsFollowRequestImpl();
};
// -------------------------------------------------------------------

class FollowReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.FollowReply) */ {
 public:
  FollowReply();
  virtual ~FollowReply();

  FollowReply(const FollowReply& from);

  inline FollowReply& operator=(const FollowReply& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  FollowReply(FollowReply&& from) noexcept
    : FollowReply() {
    *this = ::std::move(from);
  }

  inline FollowReply& operator=(FollowReply&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const FollowReply& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const FollowReply* internal_default_instance() {
    return reinterpret_cast<const FollowReply*>(
               &_FollowReply_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(FollowReply* other);
  friend void swap(FollowReply& a, FollowReply& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline FollowReply* New() const PROTOBUF_FINAL { return New(NULL); }

  FollowReply* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const FollowReply& from);
  void MergeFrom(const FollowReply& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(FollowReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // @@protoc_insertion_point(class_scope:network.FollowReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsFollowReplyImpl();
};
// -------------------------------------------------------------------

class UnfollowRequest : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.UnfollowRequest) */ {
 public:
  UnfollowRequest();
  virtual ~UnfollowRequest();

  UnfollowRequest(const UnfollowRequest& from);

  inline UnfollowRequest& operator=(const UnfollowRequest& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  UnfollowRequest(UnfollowRequest&& from) noexcept
    : UnfollowRequest() {
    *this = ::std::move(from);
  }

  inline UnfollowRequest& operator=(UnfollowRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const UnfollowRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const UnfollowRequest* internal_default_instance() {
    return reinterpret_cast<const UnfollowRequest*>(
               &_UnfollowRequest_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(UnfollowRequest* other);
  friend void swap(UnfollowRequest& a, UnfollowRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline UnfollowRequest* New() const PROTOBUF_FINAL { return New(NULL); }

  UnfollowRequest* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const UnfollowRequest& from);
  void MergeFrom(const UnfollowRequest& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(UnfollowRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // @@protoc_insertion_point(class_scope:network.UnfollowRequest)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsUnfollowRequestImpl();
};
// -------------------------------------------------------------------

class UnfollowReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.UnfollowReply) */ {
 public:
  UnfollowReply();
  virtual ~UnfollowReply();

  UnfollowReply(const UnfollowReply& from);

  inline UnfollowReply& operator=(const UnfollowReply& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  UnfollowReply(UnfollowReply&& from) noexcept
    : UnfollowReply() {
    *this = ::std::move(from);
  }

  inline UnfollowReply& operator=(UnfollowReply&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const UnfollowReply& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const UnfollowReply* internal_default_instance() {
    return reinterpret_cast<const UnfollowReply*>(
               &_UnfollowReply_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    3;

  void Swap(UnfollowReply* other);
  friend void swap(UnfollowReply& a, UnfollowReply& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline UnfollowReply* New() const PROTOBUF_FINAL { return New(NULL); }

  UnfollowReply* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const UnfollowReply& from);
  void MergeFrom(const UnfollowReply& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(UnfollowReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string name = 1;
  void clear_name();
  static const int kNameFieldNumber = 1;
  const ::std::string& name() const;
  void set_name(const ::std::string& value);
  #if LANG_CXX11
  void set_name(::std::string&& value);
  #endif
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  ::std::string* mutable_name();
  ::std::string* release_name();
  void set_allocated_name(::std::string* name);

  // @@protoc_insertion_point(class_scope:network.UnfollowReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr name_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsUnfollowReplyImpl();
};
// -------------------------------------------------------------------

class ListRequest : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.ListRequest) */ {
 public:
  ListRequest();
  virtual ~ListRequest();

  ListRequest(const ListRequest& from);

  inline ListRequest& operator=(const ListRequest& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ListRequest(ListRequest&& from) noexcept
    : ListRequest() {
    *this = ::std::move(from);
  }

  inline ListRequest& operator=(ListRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ListRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ListRequest* internal_default_instance() {
    return reinterpret_cast<const ListRequest*>(
               &_ListRequest_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    4;

  void Swap(ListRequest* other);
  friend void swap(ListRequest& a, ListRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ListRequest* New() const PROTOBUF_FINAL { return New(NULL); }

  ListRequest* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const ListRequest& from);
  void MergeFrom(const ListRequest& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(ListRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:network.ListRequest)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsListRequestImpl();
};
// -------------------------------------------------------------------

class ListReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:network.ListReply) */ {
 public:
  ListReply();
  virtual ~ListReply();

  ListReply(const ListReply& from);

  inline ListReply& operator=(const ListReply& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ListReply(ListReply&& from) noexcept
    : ListReply() {
    *this = ::std::move(from);
  }

  inline ListReply& operator=(ListReply&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ListReply& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ListReply* internal_default_instance() {
    return reinterpret_cast<const ListReply*>(
               &_ListReply_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    5;

  void Swap(ListReply* other);
  friend void swap(ListReply& a, ListReply& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ListReply* New() const PROTOBUF_FINAL { return New(NULL); }

  ListReply* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const ListReply& from);
  void MergeFrom(const ListReply& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(ListReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated string names = 1;
  int names_size() const;
  void clear_names();
  static const int kNamesFieldNumber = 1;
  const ::std::string& names(int index) const;
  ::std::string* mutable_names(int index);
  void set_names(int index, const ::std::string& value);
  #if LANG_CXX11
  void set_names(int index, ::std::string&& value);
  #endif
  void set_names(int index, const char* value);
  void set_names(int index, const char* value, size_t size);
  ::std::string* add_names();
  void add_names(const ::std::string& value);
  #if LANG_CXX11
  void add_names(::std::string&& value);
  #endif
  void add_names(const char* value);
  void add_names(const char* value, size_t size);
  const ::google::protobuf::RepeatedPtrField< ::std::string>& names() const;
  ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_names();

  // @@protoc_insertion_point(class_scope:network.ListReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::std::string> names_;
  mutable int _cached_size_;
  friend struct ::protobuf_network_2eproto::TableStruct;
  friend void ::protobuf_network_2eproto::InitDefaultsListReplyImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// FollowRequest

// string name = 1;
inline void FollowRequest::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& FollowRequest::name() const {
  // @@protoc_insertion_point(field_get:network.FollowRequest.name)
  return name_.GetNoArena();
}
inline void FollowRequest::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:network.FollowRequest.name)
}
#if LANG_CXX11
inline void FollowRequest::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:network.FollowRequest.name)
}
#endif
inline void FollowRequest::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:network.FollowRequest.name)
}
inline void FollowRequest::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:network.FollowRequest.name)
}
inline ::std::string* FollowRequest::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:network.FollowRequest.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* FollowRequest::release_name() {
  // @@protoc_insertion_point(field_release:network.FollowRequest.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void FollowRequest::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:network.FollowRequest.name)
}

// -------------------------------------------------------------------

// FollowReply

// string name = 1;
inline void FollowReply::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& FollowReply::name() const {
  // @@protoc_insertion_point(field_get:network.FollowReply.name)
  return name_.GetNoArena();
}
inline void FollowReply::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:network.FollowReply.name)
}
#if LANG_CXX11
inline void FollowReply::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:network.FollowReply.name)
}
#endif
inline void FollowReply::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:network.FollowReply.name)
}
inline void FollowReply::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:network.FollowReply.name)
}
inline ::std::string* FollowReply::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:network.FollowReply.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* FollowReply::release_name() {
  // @@protoc_insertion_point(field_release:network.FollowReply.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void FollowReply::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:network.FollowReply.name)
}

// -------------------------------------------------------------------

// UnfollowRequest

// string name = 1;
inline void UnfollowRequest::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& UnfollowRequest::name() const {
  // @@protoc_insertion_point(field_get:network.UnfollowRequest.name)
  return name_.GetNoArena();
}
inline void UnfollowRequest::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:network.UnfollowRequest.name)
}
#if LANG_CXX11
inline void UnfollowRequest::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:network.UnfollowRequest.name)
}
#endif
inline void UnfollowRequest::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:network.UnfollowRequest.name)
}
inline void UnfollowRequest::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:network.UnfollowRequest.name)
}
inline ::std::string* UnfollowRequest::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:network.UnfollowRequest.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UnfollowRequest::release_name() {
  // @@protoc_insertion_point(field_release:network.UnfollowRequest.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UnfollowRequest::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:network.UnfollowRequest.name)
}

// -------------------------------------------------------------------

// UnfollowReply

// string name = 1;
inline void UnfollowReply::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& UnfollowReply::name() const {
  // @@protoc_insertion_point(field_get:network.UnfollowReply.name)
  return name_.GetNoArena();
}
inline void UnfollowReply::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:network.UnfollowReply.name)
}
#if LANG_CXX11
inline void UnfollowReply::set_name(::std::string&& value) {
  
  name_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:network.UnfollowReply.name)
}
#endif
inline void UnfollowReply::set_name(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:network.UnfollowReply.name)
}
inline void UnfollowReply::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:network.UnfollowReply.name)
}
inline ::std::string* UnfollowReply::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:network.UnfollowReply.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UnfollowReply::release_name() {
  // @@protoc_insertion_point(field_release:network.UnfollowReply.name)
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UnfollowReply::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:network.UnfollowReply.name)
}

// -------------------------------------------------------------------

// ListRequest

// -------------------------------------------------------------------

// ListReply

// repeated string names = 1;
inline int ListReply::names_size() const {
  return names_.size();
}
inline void ListReply::clear_names() {
  names_.Clear();
}
inline const ::std::string& ListReply::names(int index) const {
  // @@protoc_insertion_point(field_get:network.ListReply.names)
  return names_.Get(index);
}
inline ::std::string* ListReply::mutable_names(int index) {
  // @@protoc_insertion_point(field_mutable:network.ListReply.names)
  return names_.Mutable(index);
}
inline void ListReply::set_names(int index, const ::std::string& value) {
  // @@protoc_insertion_point(field_set:network.ListReply.names)
  names_.Mutable(index)->assign(value);
}
#if LANG_CXX11
inline void ListReply::set_names(int index, ::std::string&& value) {
  // @@protoc_insertion_point(field_set:network.ListReply.names)
  names_.Mutable(index)->assign(std::move(value));
}
#endif
inline void ListReply::set_names(int index, const char* value) {
  GOOGLE_DCHECK(value != NULL);
  names_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:network.ListReply.names)
}
inline void ListReply::set_names(int index, const char* value, size_t size) {
  names_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:network.ListReply.names)
}
inline ::std::string* ListReply::add_names() {
  // @@protoc_insertion_point(field_add_mutable:network.ListReply.names)
  return names_.Add();
}
inline void ListReply::add_names(const ::std::string& value) {
  names_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:network.ListReply.names)
}
#if LANG_CXX11
inline void ListReply::add_names(::std::string&& value) {
  names_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:network.ListReply.names)
}
#endif
inline void ListReply::add_names(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  names_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:network.ListReply.names)
}
inline void ListReply::add_names(const char* value, size_t size) {
  names_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:network.ListReply.names)
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ListReply::names() const {
  // @@protoc_insertion_point(field_list:network.ListReply.names)
  return names_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ListReply::mutable_names() {
  // @@protoc_insertion_point(field_mutable_list:network.ListReply.names)
  return &names_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace network

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_network_2eproto__INCLUDED
