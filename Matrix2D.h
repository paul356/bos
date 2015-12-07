//
//  Matrix2D.h
//  TailorAsist
//
//  Created by user1 on 14-9-3.
//  Copyright (c) 2014年 user1. All rights reserved.
//

#import <Foundation/Foundation.h>

#define EPSILON 1e-5
#define EPSILON2 1e-10

@interface Matrix2D : NSObject
@property (nonatomic) int nrow;
@property (nonatomic) int ncol;
@property (nonatomic) NSData* data;

+ (double)vectorNorm2Square:(double*)buf length:(int)len;
+ (double)vectorNorm2:(double*)buf length:(int)len;
+ (Matrix2D*)householderMat:(Matrix2D*)vect;
+ (Matrix2D*)solveLinearRegression:(Matrix2D*)param Value:(Matrix2D*)val;
+ (Matrix2D*)identityMatrix:(int)n;

- (instancetype)initWithRowNum:(int)nRow colNum:(int)nCol;
- (void)setElement:(int)row Col:(int)col Value:(double)val;
- (double)getElement:(int)row Col:(int)col;
- (Matrix2D*)multiple:(Matrix2D*)mat;
- (void)scalarMultiple:(double)scal;
- (Matrix2D*)add:(Matrix2D*)mat;
- (Matrix2D*)upperExtendWithIdentity:(int)m;
- (Matrix2D*)transpose;
- (void)dumpContent;
@end
