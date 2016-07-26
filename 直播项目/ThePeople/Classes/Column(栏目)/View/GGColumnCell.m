//
//  GGColumnCell.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/31.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGColumnCell.h"

@implementation GGColumnCell

- (void)awakeFromNib {
    // Initialization code
}
-(void)setColumnModel:(GGColumnModel *)columnModel{
    [self.image sd_setImageWithURL:[NSURL URLWithString:columnModel.image]];
    self.name.text = columnModel.name;
}
@end
