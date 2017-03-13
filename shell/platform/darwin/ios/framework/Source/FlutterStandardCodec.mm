// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "FlutterStandardCodec_Internal.h"

#pragma mark - Codec for basic message channel

@implementation FlutterStandardMessageCodec
+ (instancetype)sharedInstance {
  static id _sharedInstance = nil;
  if (!_sharedInstance) {
    _sharedInstance = [FlutterStandardMessageCodec new];
  }
  return _sharedInstance;
}

- (NSData*)encode:(id)message {
  NSMutableData* data = [NSMutableData dataWithCapacity:32];
  FlutterStandardWriter* writer = [FlutterStandardWriter writerWithData:data];
  [writer writeValue:message];
  return data;
}

- (id)decode:(NSData*)message {
  FlutterStandardReader* reader =
      [FlutterStandardReader readerWithData:message];
  id value = [reader readValue];
  NSAssert(![reader hasMore], @"Corrupted standard message");
  return value;
}
@end

#pragma mark - Codec for method channel

@implementation FlutterStandardMethodCodec
+ (instancetype)sharedInstance {
  static id _sharedInstance = nil;
  if (!_sharedInstance) {
    _sharedInstance = [FlutterStandardMethodCodec new];
  }
  return _sharedInstance;
}

- (NSData*)encodeSuccessEnvelope:(id)result {
  NSMutableData* data = [NSMutableData dataWithCapacity:32];
  FlutterStandardWriter* writer = [FlutterStandardWriter writerWithData:data];
  [writer writeByte:0];
  [writer writeValue:result];
  return data;
}

- (NSData*)encodeErrorEnvelope:(FlutterError*)error {
  NSMutableData* data = [NSMutableData dataWithCapacity:32];
  FlutterStandardWriter* writer = [FlutterStandardWriter writerWithData:data];
  [writer writeByte:1];
  [writer writeValue:error.code];
  [writer writeValue:error.message];
  [writer writeValue:error.details];
  return data;
}

- (FlutterMethodCall*)decodeMethodCall:(NSData*)message {
  FlutterStandardReader* reader =
      [FlutterStandardReader readerWithData:message];
  id value1 = [reader readValue];
  id value2 = [reader readValue];
  NSAssert(![reader hasMore], @"Corrupted standard method call");
  NSAssert([value1 isKindOfClass:[NSString class]],
           @"Corrupted standard method call");
  return [FlutterMethodCall methodCallWithMethodName:value1 arguments:value2];
}
@end

using namespace shell;

#pragma mark - Standard serializable types

@implementation FlutterStandardTypedData
+ (instancetype)typedDataWithBytes:(NSData*)data {
  return
      [FlutterStandardTypedData typedDataWithData:data
                                             type:FlutterStandardDataTypeUInt8];
}

+ (instancetype)typedDataWithInt32:(NSData*)data {
  return
      [FlutterStandardTypedData typedDataWithData:data
                                             type:FlutterStandardDataTypeInt32];
}

+ (instancetype)typedDataWithInt64:(NSData*)data {
  return
      [FlutterStandardTypedData typedDataWithData:data
                                             type:FlutterStandardDataTypeInt64];
}

+ (instancetype)typedDataWithFloat64:(NSData*)data {
  return [FlutterStandardTypedData
      typedDataWithData:data
                   type:FlutterStandardDataTypeFloat64];
}

+ (instancetype)typedDataWithData:(NSData*)data
                             type:(FlutterStandardDataType)type {
  return [[[FlutterStandardTypedData alloc] initWithData:data type:type]
      autorelease];
}

- (instancetype)initWithData:(NSData*)data type:(FlutterStandardDataType)type {
  UInt8 elementSize = elementSizeForFlutterStandardDataType(type);
  NSAssert(data.length % elementSize == 0,
           @"Data must contain integral number of elements");
  if (self = [super init]) {
    _data = [data retain];
    _type = type;
    _elementSize = elementSize;
    _elementCount = data.length / elementSize;
  }
  return self;
}

- (void)dealloc {
  [_data release];
  [super dealloc];
}
@end

@implementation FlutterStandardBigInteger
+ (instancetype)bigIntegerWithHex:(NSString*)hex {
  return [[[FlutterStandardBigInteger alloc] initWithHex:hex] autorelease];
}

- (instancetype)initWithHex:(NSString*)hex {
  if (self = [super init]) {
    _hex = [hex retain];
  }
  return self;
}

- (void)dealloc {
  [_hex release];
  [super dealloc];
}
@end

#pragma mark - Writer and reader of standard codec

@implementation FlutterStandardWriter {
  NSMutableData* _data;
}

+ (instancetype)writerWithData:(NSMutableData*)data {
  FlutterStandardWriter* writer =
      [[FlutterStandardWriter alloc] initWithData:data];
  [writer autorelease];
  return writer;
}

- (instancetype)initWithData:(NSMutableData*)data {
  if (self = [super init]) {
    _data = [data retain];
  }
  return self;
}

- (void)dealloc {
  [_data release];
  [super dealloc];
}

- (void)writeByte:(UInt8)value {
  [_data appendBytes:&value length:1];
}

- (void)writeSize:(UInt32)size {
  if (size < 254) {
    [self writeByte:(UInt8)size];
  } else if (size <= 0xffff) {
    [self writeByte:254];
    UInt16 value = (UInt16)size;
    [_data appendBytes:&value length:2];
  } else {
    [self writeByte:255];
    [_data appendBytes:&size length:4];
  }
}

- (void)writeAlignment:(UInt8)alignment {
  UInt8 mod = _data.length % alignment;
  if (mod) {
    for (int i = 0; i < (alignment - mod); i++) {
      [self writeByte:0];
    }
  }
}

- (void)writeUTF8:(NSString*)value {
  UInt32 length = [value lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
  [self writeSize:length];
  [_data appendBytes:value.UTF8String length:length];
}

- (void)writeValue:(id)value {
  if (value == nil || value == [NSNull null]) {
    [self writeByte:FlutterStandardFieldNil];
  } else if ([value isKindOfClass:[NSNumber class]]) {
    NSNumber* number = value;
    const char* type = [number objCType];
    if (strcmp(type, @encode(BOOL)) == 0) {
      BOOL b = number.boolValue;
      [self
          writeByte:(b ? FlutterStandardFieldTrue : FlutterStandardFieldFalse)];
    } else if (strcmp(type, @encode(int)) == 0) {
      SInt32 n = number.intValue;
      [self writeByte:FlutterStandardFieldInt32];
      [_data appendBytes:(UInt8*)&n length:4];
    } else if (strcmp(type, @encode(long)) == 0) {
      SInt64 n = number.longValue;
      [self writeByte:FlutterStandardFieldInt64];
      [_data appendBytes:(UInt8*)&n length:8];
    } else if (strcmp(type, @encode(double)) == 0) {
      Float64 f = number.doubleValue;
      [self writeByte:FlutterStandardFieldFloat64];
      [_data appendBytes:(UInt8*)&f length:8];
    } else {
      NSLog(@"Unsupported value: %@", value);
      NSAssert(NO, @"Unsupported value for standard codec");
    }
  } else if ([value isKindOfClass:[NSString class]]) {
    NSString* string = value;
    [self writeByte:FlutterStandardFieldString];
    [self writeUTF8:string];
  } else if ([value isKindOfClass:[FlutterStandardBigInteger class]]) {
    FlutterStandardBigInteger* bigInt = value;
    [self writeByte:FlutterStandardFieldIntHex];
    [self writeUTF8:bigInt.hex];
  } else if ([value isKindOfClass:[FlutterStandardTypedData class]]) {
    FlutterStandardTypedData* typedData = value;
    [self writeByte:FlutterStandardFieldForDataType(typedData.type)];
    [self writeSize:typedData.elementCount];
    [self writeAlignment:typedData.elementSize];
    [_data appendData:typedData.data];
  } else if ([value isKindOfClass:[NSArray class]]) {
    NSArray* array = value;
    [self writeByte:FlutterStandardFieldList];
    [self writeSize:array.count];
    for (id object in array) {
      [self writeValue:object];
    }
  } else if ([value isKindOfClass:[NSDictionary class]]) {
    NSDictionary* dict = value;
    [self writeByte:FlutterStandardFieldMap];
    [self writeSize:dict.count];
    for (id key in dict) {
      [self writeValue:key];
      [self writeValue:[dict objectForKey:key]];
    }
  } else {
    NSLog(@"Unsupported value: %@", value);
    NSAssert(NO, @"Unsupported value for standard codec");
  }
}
@end

@implementation FlutterStandardReader {
  NSData* _data;
  NSRange _range;
}

+ (instancetype)readerWithData:(NSData*)data {
  FlutterStandardReader* reader =
      [[FlutterStandardReader alloc] initWithData:data];
  [reader autorelease];
  return reader;
}

- (instancetype)initWithData:(NSData*)data {
  if (self = [super init]) {
    _data = [data retain];
    _range = NSMakeRange(0, 0);
  }
  return self;
}

- (void)dealloc {
  [_data release];
  [super dealloc];
}

- (BOOL)hasMore {
  return _range.location < _data.length;
}

- (void)readBytes:(void*)destination length:(int)length {
  _range.length = length;
  [_data getBytes:destination range:_range];
  _range.location += _range.length;
}

- (UInt8)readByte {
  UInt8 value;
  [self readBytes:&value length:1];
  return value;
}

- (UInt32)readSize {
  UInt8 byte = [self readByte];
  if (byte < 254) {
    return (UInt32)byte;
  } else if (byte == 254) {
    UInt16 value;
    [self readBytes:&value length:2];
    return value;
  } else {
    UInt32 value;
    [self readBytes:&value length:4];
    return value;
  }
}

- (NSData*)readData:(int)length {
  _range.length = length;
  NSData* data = [_data subdataWithRange:_range];
  _range.location += _range.length;
  return data;
}

- (NSString*)readUTF8 {
  NSData* bytes = [self readData:[self readSize]];
  return [[[NSString alloc] initWithData:bytes encoding:NSUTF8StringEncoding]
      autorelease];
}

- (void)readAlignment:(UInt8)alignment {
  UInt8 mod = _range.location % alignment;
  if (mod) {
    _range.location += (alignment - mod);
  }
}

- (FlutterStandardTypedData*)readTypedDataOfType:(FlutterStandardDataType)type {
  UInt32 elementCount = [self readSize];
  UInt8 elementSize = elementSizeForFlutterStandardDataType(type);
  [self readAlignment:elementSize];
  NSData* data = [self readData:elementCount * elementSize];
  return [FlutterStandardTypedData typedDataWithData:data type:type];
}

- (id)readValue {
  FlutterStandardField field = (FlutterStandardField)[self readByte];
  switch (field) {
    case FlutterStandardFieldNil:
      return nil;
    case FlutterStandardFieldTrue:
      return @YES;
    case FlutterStandardFieldFalse:
      return @NO;
    case FlutterStandardFieldInt32: {
      SInt32 value;
      [self readBytes:&value length:4];
      return [NSNumber numberWithInt:value];
    }
    case FlutterStandardFieldInt64: {
      SInt64 value;
      [self readBytes:&value length:8];
      return [NSNumber numberWithLong:value];
    }
    case FlutterStandardFieldFloat64: {
      Float64 value;
      [self readBytes:&value length:8];
      return [NSNumber numberWithDouble:value];
    }
    case FlutterStandardFieldIntHex:
      return [FlutterStandardBigInteger bigIntegerWithHex:[self readUTF8]];
    case FlutterStandardFieldString:
      return [self readUTF8];
    case FlutterStandardFieldUInt8Data:
    case FlutterStandardFieldInt32Data:
    case FlutterStandardFieldInt64Data:
    case FlutterStandardFieldFloat64Data:
      return [self readTypedDataOfType:FlutterStandardDataTypeForField(field)];
    case FlutterStandardFieldList: {
      UInt32 length = [self readSize];
      NSMutableArray* array = [NSMutableArray arrayWithCapacity:length];
      for (UInt32 i = 0; i < length; i++) {
        id value = [self readValue];
        [array addObject:(value == nil ? [NSNull null] : value)];
      }
      return array;
    }
    case FlutterStandardFieldMap: {
      UInt32 size = [self readSize];
      NSMutableDictionary* dict =
          [NSMutableDictionary dictionaryWithCapacity:size];
      for (UInt32 i = 0; i < size; i++) {
        id key = [self readValue];
        id val = [self readValue];
        [dict setObject:(val == nil ? [NSNull null] : val)
                 forKey:(key == nil ? [NSNull null] : key)];
      }
      return dict;
    }
    default:
      NSAssert(NO, @"Corrupted standard message");
  }
}
@end
