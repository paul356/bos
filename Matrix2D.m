//
//  Matrix2D.m
//  TailorAsist
//
//  Created by user1 on 14-9-3.
//  Copyright (c) 2014年 user1. All rights reserved.
//

#import "Matrix2D.h"

@implementation Matrix2D

- (instancetype)initWithRowNum:(int)nRow colNum:(int)nCol
{
    _nrow = _ncol = 0;
    double* buf = malloc(sizeof(double)*nRow*nCol);
    if (!buf) {
        return nil;
    }
    _data = [[NSData alloc] initWithBytes:buf length:sizeof(double)*nRow*nCol];
    _nrow = nRow;
    _ncol = nCol;
    return self;
}

- (void)dumpContent
{
    double* buf = (double*)[self.data bytes];
    NSLog(@"[");
    for (int i=0; i<self.nrow; i++) {
        for (int j=0; j<self.ncol; j++) {
            NSLog(@"%lf, ", *buf);
            buf += 1;
        }
        if (i != self.nrow-1)
            NSLog(@"\n");
    }
    NSLog(@"]\n");
}

- (Matrix2D*)multiple:(Matrix2D*)mat
{
    if (self.ncol != mat.nrow) {
        return nil;
    }
    
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:self.nrow colNum:mat.ncol];
    if (!res) {
        return res;
    }
    
    double* lbuf = (double*)[self.data bytes];
    double* rbuf = (double*)[mat.data bytes];
    double* sbuf = (double*)[res.data bytes];
    
    for (int i=0; i<self.nrow; i++) {
        for (int j=0; j<mat.ncol; j++) {
            double val = 0.0;
            for (int k=0; k<self.ncol; k++) {
                val += lbuf[i*self.ncol + k] * rbuf[k*mat.ncol + j];
            }
            sbuf[i*mat.ncol + j] = val;
        }
    }
    
    return res;
}

- (void)scalarMultiple:(double)scal
{
    double* buf = (double *)[self.data bytes];
    
    for (int i=0; i<self.nrow; i++) {
        for (int j=0; j<self.ncol; j++) {
            buf[i*self.ncol + j] *= scal;
        }
    }
}

- (Matrix2D*)add:(Matrix2D*)mat
{
    if (self.nrow != mat.nrow || self.ncol != mat.ncol) {
        return nil;
    }
    
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:self.nrow colNum:self.ncol];
    if (!res) {
        return res;
    }
    
    double* lbuf = (double*)[self.data bytes];
    double* rbuf = (double*)[mat.data bytes];
    double* sbuf = (double*)[res.data bytes];
    
    for (int i=0; i<self.nrow; i++) {
        for (int j=0; j<self.ncol; j++) {
            sbuf[i*self.ncol + j] = lbuf[i*self.ncol + j] + rbuf[i*self.ncol + j];
        }
    }
    
    return res;
}

+ (Matrix2D*)identityMatrix:(int)n
{
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:n colNum:n];
    if (!res) {
        return res;
    }
    
    double* sbuf = (double*)[res.data bytes];
    
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            if (i == j) {
                sbuf[i*n + j] = 1.0;
            } else {
                sbuf[i*n + j] = 0.0;
            }
        }
    }
    return res;
}

- (Matrix2D*)subVectorForLR:(int)idx
{
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:self.nrow-idx colNum:1];
    if (!res) {
        return res;
    }
    
    double* sbuf = (double*)[self.data bytes];
    double* dbuf = (double*)[res.data bytes];
    
    for (int i=idx; i<self.nrow; i++) {
        dbuf[i-idx] = sbuf[i*self.ncol + idx];
    }
    
    return res;
}

- (double)getElement:(int)row Col:(int)col
{
    double* buf = (double*)[self.data bytes];
    return buf[row*self.ncol + col];
}

- (void)setElement:(int)row Col:(int)col Value:(double)val
{
    double* buf = (double*)[self.data bytes];
    buf[row*self.ncol + col] = val;
}

- (Matrix2D*)transpose
{
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:self.ncol colNum:self.nrow];
    if (!res) {
        return res;
    }
    
    double* sbuf = (double*)[self.data bytes];
    double* dbuf = (double*)[res.data bytes];
    
    for (int i=0; i<self.nrow; i++) {
        for (int j=0; j<self.ncol; j++) {
            dbuf[j*self.nrow + i] = sbuf[j];
        }
        sbuf += self.ncol;
    }
    
    return res;
}

+ (double)vectorNorm2Square:(double*)buf length:(int)len
{
    double res = 0.0;
    for (int i=0; i<len; i++) {
        res += buf[i]*buf[i];
    }
    return res;
}

+ (double)vectorNorm2:(double*)buf length:(int)len
{
    return sqrt([Matrix2D vectorNorm2Square:buf length:len]);
}

+ (Matrix2D*)householderMat:(Matrix2D*)vect
{
    double subVecNorm = [Matrix2D vectorNorm2:(double *)[vect.data bytes] length:vect.nrow];
    double saveVal    = [vect getElement:0 Col:0];
    [vect setElement:0 Col:0 Value:saveVal+subVecNorm];
    double normSquare = [Matrix2D vectorNorm2Square:(double*)[vect.data bytes] length:vect.nrow];
    if (normSquare <= EPSILON*EPSILON) {
        NSLog(@"Matrix is degraded\n");
        return nil;
    }
    Matrix2D* vecMutiVecT = [vect multiple:[vect transpose]];
    [vecMutiVecT scalarMultiple:-2.0/normSquare];
    Matrix2D* qi = [[Matrix2D identityMatrix:vect.nrow] add:vecMutiVecT];
    // Revert to original value
    [vect setElement:0 Col:0 Value:saveVal];
    return qi;
}

+ (Matrix2D*)solveLinearRegression:(Matrix2D*)param Value:(Matrix2D*)val
{
    if (param.nrow < param.ncol) {
        return nil;
    }
    if (val.nrow != param.nrow || val.ncol != 1) {
        return nil;
    }
    
    for (int i=0; i<param.ncol - 1; i++) {
        Matrix2D* subVec = [param subVectorForLR:i];
        Matrix2D* qi = [Matrix2D householderMat:subVec];
        if (i) {
            qi = [qi upperExtendWithIdentity:i];
        }
        [qi dumpContent];
        param = [qi multiple:param];
        val = [qi multiple:val];
    }
    
    double chkElmt = [param getElement:param.ncol-1 Col:param.ncol-1];
    if (chkElmt*chkElmt <= EPSILON2) {
        NSLog(@"Matrix is degraded\n");
        return nil;
    }
    
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:param.ncol colNum:1];
    if (!res) {
        return nil;
    }
    double* resData = (double*)[res.data bytes];
    double* valData = (double*)[val.data bytes];
    double* paraData = (double*)[param.data bytes] + (param.ncol-1)*param.ncol;
    
    for (int i=param.ncol-1; i>=0; i--) {
        double paramSum = 0.0;
        for (int j=i+1; j<param.ncol; j++) {
            paramSum += resData[j] * paraData[j];
        }
        resData[i] = (valData[i] - paramSum)/paraData[i];
        
        paraData -= param.ncol;
    }
    
    return res;
}

- (Matrix2D*)upperExtendWithIdentity:(int)m
{
    Matrix2D* res = [[Matrix2D alloc] initWithRowNum:self.nrow + m colNum:self.ncol + m];
    if (!res) {
        return res;
    }
    
    double* sbuf = (double*)[self.data bytes];
    double* dbuf = (double*)[res.data bytes];
    
    for (int i=0; i<res.nrow; i++) {
        for (int j=0; j<res.ncol; j++) {
            if (i < m && j == i) {
                dbuf[i*res.ncol + j] = 1.0;
            } else if (i < m || j < m) {
                dbuf[i*res.ncol + j] = 0.0;
            } else {
                dbuf[i*res.ncol + j] = sbuf[(i-m)*self.ncol + (j-m)];
            }
        }
    }
    
    return res;
}

@end
